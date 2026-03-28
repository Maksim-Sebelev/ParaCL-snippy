module;

#include <cstdlib>
#include <random>
#include <cstdlib>
#include <random>
#include <stdexcept>
#include <vector>
#include <array>

#include "create-basic-node.hpp"

export module ast_builder;

export import thelast;
export import test_generator_settings;
import name_generator;
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

    std::size_t statement_depth_ = 0;
    static constexpr std::size_t max_statement_depth_ = 3;

    using generate_stmt_t = BasicNode (AstGenerator::*)();

    BasicNode generate_new_placeholder_name()
    {
        return name_generator_.generate_new_variable();
        // return last::node::create(test_generator::UninitializedNameDeclaration{});
    }

    BasicNode generate_existing_placeholder_name()
    {
        return name_generator_ .generate_existing_variable();
        // return last::node::create(test_generator::Uninitiali777zedNameReUse{});
    }

    void reset_continue_expression_probability()
    {
        continue_expression_probability_ = settings_.continue_expression_max_probability;
    }

    void update_continue_expression_probability()
    {
        continue_expression_probability_ *= 0.5;
    }

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
          continue_expression_probability_    (settings_.continue_expression_max_probability) {}

private:
    BasicNode generate_scope()
    {
        std::vector<BasicNode> statements;
        do
        {
            statements.push_back(generate_statement());
        }
        while (will_generate_new_statement());

        return last::node::create(last::node::Scope{std::move(statements)});
    }

    BasicNode generate_statement()
    {
        auto generator = select_random_stmt();
        return (this->*generator)();
    }

    generate_stmt_t select_random_stmt()
    {
        auto weights = settings_.statements_weights;

        // TODO: Кирюха потом доделает
        weights[static_cast<std::size_t>(Statement::PrintStm)] = 0;
        weights[static_cast<std::size_t>(Statement::InStm)]    = 0;

        weight_t total_weight = 0;
        for (auto w : weights)
            total_weight += w;

        if (total_weight == 0)
            throw std::runtime_error("All statement weights are zero");

        std::discrete_distribution<std::size_t> dist(
            weights.begin(),
            weights.end()
        );

        auto index = dist(random_);

        switch (static_cast<Statement>(index))
        {
            case Statement::ExpressionStmt:
                return &AstGenerator::generate_expression;

            case Statement::WhileStmt:
                return &AstGenerator::generate_while;

            case Statement::IfStmt:
                return &AstGenerator::generate_if;

            case Statement::AssignmentStmt:
                return &AstGenerator::generate_assigment;

            case Statement::PrintStm:
                return &AstGenerator::generate_print;

            case Statement::InStm:
                return &AstGenerator::generate_in;

            case Statement::STATEMENTS_SIZE:
                __builtin_unreachable();
            // break;
            default:
                return &AstGenerator::generate_assigment;
        }

        throw std::runtime_error("Invalid statement kind");
    }

private:
    BasicNode generate_while()
    {
        if (statement_depth_ >= max_statement_depth_)
            return generate_assigment();

        ++statement_depth_;

        auto condition = generate_binary_operator();
        auto body      = generate_scope();

        --statement_depth_;

        return last::node::create(last::node::While{
            std::move(condition),
            std::move(body)
        });
    }

    BasicNode generate_if()
    {
        if (statement_depth_ >= max_statement_depth_)
            return generate_assigment();

        ++statement_depth_;

        auto condition = generate_binary_operator();
        auto body      = generate_scope();

        --statement_depth_;

        return last::node::create(last::node::If{
            std::move(condition),
            std::move(body)
        });
    }

    BasicNode generate_assigment()
    {
        auto lhs = generate_new_placeholder_name();
        auto rhs = generate_expression();

        return last::node::create(last::node::BinaryOperator{
            last::node::BinaryOperator::ASGN,
            std::move(lhs),
            std::move(rhs)
        });
    }

    BasicNode generate_print()
    {
        std::uniform_int_distribution<std::size_t> argc_dist(1, 3);
        auto argc = argc_dist(random_);

        std::vector<BasicNode> args;
        args.reserve(argc);

        for (std::size_t i = 0; i < argc; ++i)
            args.push_back(generate_expression());

        return last::node::create(last::node::Print{std::move(args)});
    }

    BasicNode generate_in()
    {
        auto lhs = generate_new_placeholder_name();
        auto rhs = last::node::create(last::node::Scan{});

        return last::node::create(last::node::BinaryOperator{
            last::node::BinaryOperator::ASGN,
            std::move(lhs),
            std::move(rhs)
        });
    }

    // BasicNode generate_print()
    // {
    //     throw std::runtime_error("generate_print is not implemented yet");
    // }

    // BasicNode generate_in()
    // {
    //     throw std::runtime_error("generate_in is not implemented yet");
    // }

    BasicNode generate_expression() // первая легкаяв версия
    {
        reset_continue_expression_probability();

        if (!will_continue_expression())
        {
            std::uniform_int_distribution<int> dist(-10, 10);
            return last::node::create(last::node::NumberLiteral{dist(random_)});
        }

        weight_t total_weight = 0;
        for (auto w : settings_.expression_weights)
            total_weight += w;

        if (total_weight == 0)
            throw std::runtime_error("All expression weights are zero");

        std::discrete_distribution<std::size_t> dist(
            settings_.expression_weights.begin(),
            settings_.expression_weights.end()
        );

        auto index = dist(random_);

        switch (static_cast<Expression>(index))
        {
            case Expression::AssignmentExpr:
                return generate_assigment();

            case Expression::BinaryOperatorExpr:
                return generate_binary_operator();

            case Expression::UnaryOperatorExpr:
                return generate_unary_operator();

            case Expression::InExpr:
                return generate_in();
                //throw std::runtime_error("InExpr is not implemented yet");

            case Expression::PrintExpr:
                return generate_print();
                // throw std::runtime_error("PrintExpr is not implemented yet");

            case Expression::EXPRESSIONS_SIZE:
                break;
        }

        throw std::runtime_error("Invalid expression kind");
    }

    BasicNode generate_binary_operator()
    {
        auto lhs = last::node::create(last::node::NumberLiteral{1});
        auto rhs = last::node::create(last::node::NumberLiteral{2});

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

        return last::node::create(last::node::BinaryOperator{
            op,
            std::move(lhs),
            std::move(rhs)
        });
    }

    BasicNode generate_unary_operator()
    {
        auto arg = last::node::create(last::node::NumberLiteral{1});

        std::array<last::node::UnaryOperator::UnaryOperatorT, 3> ops =
        {
            last::node::UnaryOperator::PLUS,
            last::node::UnaryOperator::MINUS,
            last::node::UnaryOperator::NOT
        };

        std::uniform_int_distribution<std::size_t> dist(0, ops.size() - 1);
        auto op = ops[dist(random_)];

        update_continue_expression_probability();

        return last::node::create(last::node::UnaryOperator{
            op,
            std::move(arg)
        });
    }

public:
    last::AST generate_random_ast()
    {
        auto root = generate_scope();

        test_generator::NameGenerator name_generator(random_);
        // test_generator::name_inserter::insert_names(root, name_generator);

        return last::AST(std::move(root));
    }
};

export
last::AST generate_random_ast(SnippySettings const & settings)
{
    return AstGenerator{settings}.generate_random_ast();
}

} /* namespace test_generator */

