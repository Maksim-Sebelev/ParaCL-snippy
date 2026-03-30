module;

#include <cstdlib>
#include <random>
#include <cstdlib>
#include <random>
#include <stdexcept>
#include <vector>
#include <array>
#include <cassert>
#include <iostream>

#include "create-basic-node.hpp"

export module ast_builder;

export import thelast;
export import test_generator_settings;
import name_generator;
import ast_serializer;


SPECIALIZE_CREATE(last::node::Scope              , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::Print              , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::Scan               , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::UnaryOperator      , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::BinaryOperator     , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::If                 , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::Else               , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::Condition          , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::While              , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::NumberLiteral      , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::StringLiteral      , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::Return             , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::FunctionCall       , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::FunctionDeclaration, last::node::serializable, last::node::dumpable)

namespace test_generator
{

using BasicNode = last::node::BasicNode;


class AstGenerator
{
private:
    SnippySettings  settings_;
    std::mt19937    random_;
    NameGenerator   name_generator_;

    probability_t generate_next_statement_probability_;
    probability_t continue_expression_probability_;

    std::size_t statement_depth_  = 0LU;
    std::size_t expression_depth_ = 0LU;

private:
    void check_configuration()
    {
        auto&& has_statement_weight = false;
        for (auto&& w : settings_.statements_weights)
        {
            if (w != 0)
            {
                has_statement_weight = true;
                break;
            }
        }

        if (!has_statement_weight)
            throw std::runtime_error("All statement weights are zero. Incorrect generator configuration.");

        auto&& has_expression_weight = false;
        for (auto&& w : settings_.expressions_weights)
        {
            if (w != 0)
            {
                has_expression_weight = true;
                break;
            }
        }

        if (!has_expression_weight)
            throw std::runtime_error("All expression weights are zero. Incorrect generator configuration.");

        auto&& has_terminal_expression =
            settings_.expressions_weights[Expression::NumberLiteralExpr] != 0 or
            settings_.expressions_weights[Expression::InExpr           ] != 0;

        if (not has_terminal_expression)
            throw std::runtime_error(
                "Both VariableExpr and NumberLiteralExpr weights are zero.\n"
                "Terminal expressions are disabled, so the generator is incorrectly configured.\n"
                "Also, only enabled Variables are useless, cause it`s impossible to use only variables, without other terminal expressions."
            );
    }

    BasicNode generate_terminal_expression()
    {
        auto&& expressions = std::vector<Expression>{};
        auto&& weights     = std::vector<weight_t>{};

        auto&& add_expression_if_its_possible = [&](Expression id) -> void
        {
            if (settings_.expressions_weights[id] == 0) return;
            if ((id == Expression::VariableExpr) and name_generator_.empty()) return;

            expressions.push_back(id);
            weights.push_back(settings_.expressions_weights[id]);
        };

        add_expression_if_its_possible(Expression::NumberLiteralExpr);
        add_expression_if_its_possible(Expression::VariableExpr);
        add_expression_if_its_possible(Expression::InExpr);

        assert(not expressions.empty());

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        auto&& chosen = expressions[dist(random_)];

        switch (chosen)
        {
            case Expression::NumberLiteralExpr:
                return generate_number();

            case Expression::VariableExpr:
                return generate_variable();

            case Expression::InExpr:
                return generate_in();

            default:
                throw std::runtime_error("Invalid terminal expression selected");
        }
        __builtin_unreachable();
    }


    BasicNode generate_unterminal_expression()
    {
        auto&& expressions = std::vector<Expression>{};
        auto&& weights     = std::vector<weight_t>{};

        auto&& add_expression_if_its_possible = [&](Expression id) -> void
        {
            if (settings_.expressions_weights[id] == 0) return;

            expressions.push_back(id);
            weights.push_back(settings_.expressions_weights[id]);
        };

        add_expression_if_its_possible(Expression::BinaryOperatorExpr);
        add_expression_if_its_possible(Expression::UnaryOperatorExpr);
        add_expression_if_its_possible(Expression::PrintExpr);
        add_expression_if_its_possible(Expression::AssignmentExpr);

        if (expressions.empty())
            return BasicNode{};

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        auto&& chosen = expressions[dist(random_)];

        switch (chosen)
        {
            case Expression::BinaryOperatorExpr:
                return generate_binary_operator();

            case Expression::UnaryOperatorExpr:
                return generate_unary_operator();

            case Expression::PrintExpr:
                return generate_print();

            case Expression::AssignmentExpr:
                return generate_assign();

            default:
                throw std::runtime_error("Invalid terminal expression selected");
        }
        return BasicNode{};
    }

    void reset_continue_expression_probability()
    {
        expression_depth_                = 0;
        std::uniform_real_distribution<probability_t> dist(0.0, settings_.continue_expression_max_probability);
        continue_expression_probability_ = dist(random_);
    }


    void update_continue_expression_probability() // TODO: сделать по нормальному, когда параметры будут не захардкожены
    {
        if (expression_depth_ >= settings_.max_expression_depth)
        {
            continue_expression_probability_ = 0.0;
            return;
        }

        auto remaining_depth =
            static_cast<double>(settings_.max_expression_depth - expression_depth_);
        auto total_depth     =
            static_cast<double>(settings_.max_expression_depth);

        continue_expression_probability_=
            settings_.continue_expression_max_probability * (remaining_depth / total_depth);
    }
// depth = 0 => 0.8
// depth = 1 => 0.6
// depth = 2 => 0.4
// depth = 3 => 0.2
// depth = 4 => 0.0

    bool will_generate_new_statement()
    {
        auto dist = std::bernoulli_distribution{generate_next_statement_probability_};
        return dist(random_);
    }

    bool will_continue_expression()
    {
        auto dist = std::bernoulli_distribution{continue_expression_probability_};
        return dist(random_);
    }

public:
    AstGenerator(SnippySettings const& settings)
        : settings_(settings),
        random_(std::random_device{}()),
        name_generator_(random_),
        generate_next_statement_probability_(settings_.generate_next_statement_probability),
        continue_expression_probability_    (settings_.continue_expression_max_probability)
    {
        check_configuration();
    }

private:
    BasicNode generate_scope()
    {
        if (statement_depth_ > settings_.max_scope_depth)
            return generate_not_scoped_statement();

        name_generator_.new_scope();
        ++statement_depth_;

        auto&& statements = std::vector<BasicNode>{};
        do
        {
            statements.push_back(generate_statement());
        }
        while (will_generate_new_statement());

        name_generator_.leave_scope();
        --statement_depth_;

        return last::node::create(last::node::Scope{std::move(statements)});
    }

    BasicNode generate_not_scoped_statement()
    {
        auto&& statements = std::vector<Statement>{};
        auto&& weights     = std::vector<weight_t>{};

        auto&& add_expression_if_its_possible = [&](Statement id) -> void
        {
            if (settings_.statements_weights[id] == 0) return;

            statements.push_back(id);
            weights.push_back(settings_.statements_weights[id]);
        };

        add_expression_if_its_possible(Statement::PrintStmt);
        add_expression_if_its_possible(Statement::VariableDeclarationStmt);
        add_expression_if_its_possible(Statement::ExpressionStmt);

        if (statements.empty())
            throw std::runtime_error("Requests not scoped statement, but no one is enabled");

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        auto&& chosen = statements[dist(random_)];

        switch (chosen)
        {
            case Statement::PrintStmt:
                return generate_print();

            case Statement::VariableDeclarationStmt:
                return generate_assign();

            case Statement::ExpressionStmt:
                return generate_expression();

            default:
                throw std::runtime_error("Invalid terminal expression selected");
        }
        __builtin_unreachable();
    }

    BasicNode generate_statement()
    {
        auto&& weights = settings_.statements_weights;

        std::discrete_distribution<std::size_t> dist(
            weights.begin(),
            weights.end()
        );

        auto&& selected_id = static_cast<Statement>(dist(random_));

        switch (selected_id)
        {
            case Statement::ExpressionStmt:
                return generate_expression();

            case Statement::WhileStmt:
                return generate_while();

            case Statement::IfStmt:
                return generate_if();

            case Statement::VariableDeclarationStmt:
                return generate_assign();

            case Statement::PrintStmt:
                return generate_print();

            case Statement::ScopeStmt:
                return generate_scope();

#if not defined(NDEBUG)
            case Statement::STATEMENTS_SIZE:
                throw std::runtime_error("Statement::STATEMENTS_SIZE is unexpected here.");
            default:
                throw std::runtime_error("Undefined statement id: " + std::to_string(selected_id));
#else /* not defined(NDEBUG) */
            default:
                __builtin_unreachable();
#endif /* not defined(NDEBUG) */
        }

#if not defined(NDEBUG)
        throw std::runtime_error("Invalid statement kind");
#else /* not defined(NDEBUG) */
        __builtin_unreachable();
#endif /* not defined(NDEBUG) */

    }

private:
    BasicNode generate_while()
    {
        if (statement_depth_ >= settings_.max_scope_depth)
            return generate_not_scoped_statement();

        auto condition = generate_expression();
        auto body      = generate_scope();

        return last::node::create(last::node::While{
            std::move(condition),
            std::move(body)
        });
    }

    BasicNode generate_if()
    {
        if (statement_depth_ >= settings_.max_scope_depth)
            return generate_not_scoped_statement();

        auto condition = generate_expression();
        auto body      = generate_scope();

        auto if_node = last::node::create(last::node::If{
            std::move(condition),
            std::move(body)
        });

        return last::node::create(last::node::Condition({std::move(if_node)}, {}));
    }

    BasicNode generate_assign()
    {
        // generating expression before,
        // cause if variables will be declared before, 
        // generator will use this variable
        // (for exmample can be situation:
        // var = var + 1
        // but var was not declared in this statement
    
        auto&& value        = generate_expression();

        auto&& kind_dist    = std::uniform_int_distribution<int> (0, 1);
        auto&& kind         = kind_dist(random_);
        auto&& new_variable = (kind == 0);
        auto&& variable     = (new_variable) ? name_generator_.generate_new_variable () : name_generator_.generate_absolute_new_variable ();

        return last::node::create(last::node::BinaryOperator
        {
            last::node::BinaryOperator::ASGN,
            std::move(variable),
            std::move(value)
        });
    }

    BasicNode generate_print()
    {
        std::uniform_int_distribution<std::size_t> argc_dist(1, 3);
        auto argc = argc_dist(random_);

        std::vector<BasicNode> args = {generate_expression()};

        return last::node::create(last::node::Print{std::move(args)});
    }

    BasicNode generate_in()
    {
        return last::node::create(last::node::Scan{});
    }

    BasicNode generate_number()
    {
        return last::node::create(last::node::NumberLiteral{std::uniform_int_distribution<int>(-100, 100)(random_)});
    }

    BasicNode generate_variable(unique_name_id_t id = NameGenerator::RandomExistingName)
    {
        if (name_generator_.empty()) throw std::runtime_error("Requests generation of variable when exists no variables yet");
        return name_generator_.generate_existing_variable(id);
    }

    BasicNode generate_expression()
    {
        reset_continue_expression_probability();
        return generate_expression_impl();
    }

    BasicNode generate_expression_impl()
    {
        auto&& cut_off_expression = (not will_continue_expression()) or (expression_depth_ >= settings_.max_expression_depth);

        if (cut_off_expression)
            return generate_terminal_expression();

        /* if (not unterminal_expression) - exists no unterminal expression with positive weights,
           so we can only generate terminal expression */

        auto&& unterminal_expression = generate_unterminal_expression();
        if (unterminal_expression) return unterminal_expression;
    
        return generate_terminal_expression();
    }

    BasicNode generate_binary_operator()
    {
        if (expression_depth_ >= settings_.max_expression_depth)
            return generate_terminal_expression();

        ++expression_depth_;

        BasicNode lhs;
        BasicNode rhs;

        if (will_continue_expression())
            lhs = generate_expression_impl();
        else
            lhs = generate_terminal_expression();

        if (will_continue_expression())
            rhs = generate_expression_impl();
        else
            rhs = generate_terminal_expression();

        std::array<last::node::BinaryOperator::BinaryOperatorT, 6> ops =
        {
            last::node::BinaryOperator::ADD,
            last::node::BinaryOperator::SUB,
            last::node::BinaryOperator::MUL,
            last::node::BinaryOperator::DIV,
            last::node::BinaryOperator::ISEQ,
            last::node::BinaryOperator::ISNE
        };

        std::uniform_int_distribution<std::size_t> dist(0, ops.size() - 1);
        auto op = ops[dist(random_)];

        update_continue_expression_probability();
        --expression_depth_;

        return last::node::create(last::node::BinaryOperator{
            op,
            std::move(lhs),
            std::move(rhs)
        });
    }

    BasicNode generate_unary_operator()
    {
        if (expression_depth_ >= settings_.max_expression_depth)
            return generate_terminal_expression();

        ++expression_depth_;

        BasicNode arg;
        if (will_continue_expression())
            arg = generate_expression_impl();
        else
            arg = generate_terminal_expression();

        std::array<last::node::UnaryOperator::UnaryOperatorT, 3> ops =
        {
            last::node::UnaryOperator::PLUS,
            last::node::UnaryOperator::MINUS,
            last::node::UnaryOperator::NOT
        };

        std::uniform_int_distribution<std::size_t> dist(0, ops.size() - 1);
        auto op = ops[dist(random_)];

        update_continue_expression_probability();
        --expression_depth_;

        return last::node::create(last::node::UnaryOperator{
            op,
            std::move(arg)
        });
    }

public:
    last::AST generate_random_ast()
    {
        auto root = generate_scope();

        // test_generator::NameGenerator name_generator(random_);

        return last::AST(std::move(root));
    }
};

export
last::AST generate_random_ast(SnippySettings const & settings)
{
    return AstGenerator{settings}.generate_random_ast();
}

} /* namespace test_generator */
