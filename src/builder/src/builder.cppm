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
export import settings;
import name_generator;
import ast_serializer;


SPECIALIZE_CREATE(last::node::Scope              , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::LinearSequence     , last::node::serializable, last::node::dumpable)
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
SPECIALIZE_CREATE(last::node::Comment            , last::node::serializable, last::node::dumpable)
SPECIALIZE_CREATE(last::node::Semicolon          , last::node::serializable, last::node::dumpable)

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

    std::size_t scope_depth_        = 0LU;
    std::size_t expression_depth_   = 0LU;
    std::size_t statements_counter_ = 0LU;
    std::size_t function_depth_     = 0LU;
    std::size_t next_function_id_   = 0LU;

    std::vector<std::string> function_names_;

    bool inside_function_ : 1 = false;

    bool делаем_пасхалку_ : 1;
    std::vector<std::string_view> пасхалка_
    {
        "Ослепший старый маг",
        "Ночью по лесу бродил",
        "На кладбище разлил",
        "Он волшебный эликсир",
        "И лишь проговорил",
        "\"Что ж я, старый, натворил!\"",
        "Хой!",
        "Хой, хой!",
        "Пого-пого!",
        "Трупы оживали, землю разрывали",
        "Всюду выползали, дико бушевали",
        "Глотки драли, всё вокруг ломали",
        "Рвали свою плоть",
        "Это место, люди не любили",
        "Потому что здесь гадов хоронили",
        "Все они водку пили",
        "Проклятыми были",
        "Среди ублюдков шёл артист",
        "В кожаном плаще, мёртвый анархист",
        "Крикнул он: \"Хой! Челюсть долой!\"",
        "Трупов вёл он за собой",
        "Хой, хой!",
        "Пого-пого!",
        "Бешено в деревню мёртвые ворвались!",
        "В свете фонарей рожи показались",
        "Дрались и по полной отрывались",
        "Шли дома громить",
        "Взяли люди топоры и вилы",
        "Мертвецов загнать в свои могилы",
        "Но на это не хватило силы",
        "Трупов не убить!",
        "Среди ублюдков шёл артист",
        "В кожаном плаще, мёртвый анархист",
        "Крикнул он: \"Хой! Челюсть долой!\"",
        "Трупов вёл он за собой",
        "Был на руке застывший фак",
        "Из кармана торчал пиратский флаг",
        "Зомби всю ночь кричали: \"Хой!",
        "Мы, анархисты, народ не злой!\"",
        "Что за наваждение! Без предупреждения",
        "На отрока напали, сильно напугали",
        "Смеялись и толкались, парнишку заставляли",
        "Пого танцевать!",
        "Что есть мочи женщины визжали",
        "И крестьяне в панике бежали",
        "Трупы дохли и снова оживали",
        "Ржали людям вслед",
        "Среди ублюдков шёл артист",
        "В кожаном плаще, мёртвый анархист",
        "Крикнул он: \"Хой! Челюсть долой!\"",
        "Трупов вёл он за собой",
        "Был на руке застывший фак",
        "Из кармана торчал пиратский флаг",
        "Зомби всю ночь кричали: \"Хой!",
        "Мы, анархисты, народ не злой!\"",
        "В жизни артист весёлым был",
        "И нажраться он всегда любил",
        "Утро крестьянам помогло",
        "Солнце трупы за полчаса сожгло",
        "Но в тишине ночной",
        "В подвале кто-то рявкнул",
        "\"Хой!\"",
        "__перерыв__",
        "За столом сидели мужики и ели",
        "Мясом конюх угощал своих гостей",
        "Все расхваливали ужин, и хозяин весел был",
        "О жене своей всё время говорил",
        "Ели мясо мужики",
        "Пивом запивали",
        "О чём конюх говорил",
        "Они не понимали",
        "\"Я узнал недавно, все вы, как ни странно",
        "Конюх хриплым голосом проговорил",
        "С моей бабою встречались в тайне от меня",
        "И поэтому всех вас собрал сегодня я\"",
        "Ели мясо мужики",
        "Пивом запивали",
        "О чём конюх говорил",
        "Они не понимали",
        "\"Я за ней не уследил",
        "В том моя вина",
        "Но скажите",
        "Правда, вкусная она?\"",
        "Ели мясо мужики",
        "Пивом запивали",
        "О чём конюх говорил",
        "Они не понимали",
        "Ели мясо мужики",
        "Пивом запивали",
        "О чём конюх говорил",
        "Они не понимали",
        "__перерыв__",
        "Замученный дорогой, я выбился из сил",
        "И в доме лесника я ночлега попросил",
        "С улыбкой добродушной старик меня впустил",
        "И жестом дружелюбным на ужин пригласил, хэй!",
        "Будь как дома, путник, я ни в чём не откажу",
        "Я ни в чём не откажу, я ни в чём не откажу, хэй!",
        "Множество историй, коль желаешь, расскажу",
        "Коль желаешь, расскажу, коль желаешь, расскажу",
        "На улице темнело, сидел я за столом",
        "Лесник сидел напротив, болтал о том, о сём",
        "Что нет среди животных у старика врагов",
        "Что нравится ему подкармливать волков, хэй!",
        "Будь как дома, путник, я ни в чём не откажу",
        "Я ни в чём не откажу, я ни в чём не откажу. хэй!",
        "Множество историй, коль желаешь, расскажу",
        "Коль желаешь, расскажу, коль желаешь, расскажу",
        "И волки среди ночи завыли под окном",
        "Старик заулыбался и вдруг покинул дом",
        "Но вскоре возвратился с ружьём наперевес",
        "\"Друзья хотят покушать, пойдём, приятель, в лес!\"",
        "Будь как дома, путник, я ни в чём не откажу",
        "Я ни в чём не откажу, я ни в чём не откажу. хэй!",
        "Множество историй, коль желаешь, расскажу",
        "Коль желаешь, расскажу, коль желаешь, расскажу",
    };

private:
    BasicNode generate_semicolon()
    {
        auto&& kind_dist = std::uniform_int_distribution<int> (0, 1);
        auto&& kind      = static_cast<bool>(kind_dist(random_));
        return last::node::create(last::node::Semicolon{kind});
    }

    BasicNode generate_comment()
    {
        auto&& kind_dist = std::uniform_int_distribution<int> (0, 1);
        auto&& separated = static_cast<bool>(kind_dist(random_));

        if (not делаем_пасхалку_)
        {
            auto&& kind_type = kind_dist(random_);

            auto&& type = (kind_type == 0) ? last::node::Comment::MultiLine : last::node::Comment::OneLine;
            auto&& comment = (kind_type == 0) ? (separated ? "\njust\nmulti-line\ncomment" : "just multi-line comment") : " just one line comment";
            return last::node::create(last::node::Comment(type, std::move(comment), separated));
        }

        static size_t counter = 0LU;
        auto&& comment = " " + std::string(пасхалка_[counter++]);
        if (counter >= пасхалка_.size()) делаем_пасхалку_ = false;

        return last::node::create(last::node::Comment{last::node::Comment::OneLine, std::move(comment), separated});
    }

    BasicNode generate_terminal_expression()
    {
        auto&& expressions = std::vector<Expression>{};
        auto&& weights     = std::vector<weight_t>{};

        auto&& add_expression_if_its_possible = [&](Expression id) -> void
        {
            if (not enabled(id)) return;
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
            case Expression::NumberLiteralExpr: return generate_number();
            case Expression::VariableExpr     : return generate_variable();
            case Expression::InExpr           : return generate_in();
            default: break;
        }

#if defined(NDEBUG)
        __builtin_unreachable();
#else /* defined(NDEBUG) */
        throw std::runtime_error("Invalid terminal expression selected");
#endif /* defined(NDEBUG) */
    }


    BasicNode generate_unterminal_expression()
    {
        auto&& expressions = std::vector<Expression>{};
        auto&& weights     = std::vector<weight_t>{};

        auto&& add_expression_if_its_possible = [&](Expression id) -> void
        {
            if (not enabled(id)) return;

            expressions.push_back(id);
            weights.push_back(settings_.expressions_weights[id]);
        };

        add_expression_if_its_possible(Expression::BinaryOperatorExpr);
        add_expression_if_its_possible(Expression::UnaryOperatorExpr);
        add_expression_if_its_possible(Expression::PrintExpr);
        add_expression_if_its_possible(Expression::FunctionCallExpr);

        if (expressions.empty())
            return BasicNode{};

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        auto&& chosen = expressions[dist(random_)];

        switch (chosen)
        {
            case Expression::BinaryOperatorExpr     : return generate_binary_operator     ();
            case Expression::UnaryOperatorExpr      : return generate_unary_operator      ();
            case Expression::PrintExpr              : return generate_print               ();
            case Expression::FunctionCallExpr       : return generate_function_call       ();
            default: break;
        }
#if defined(NDEBUG)
        __builtin_unreachable();
#else /* defined(NDEBUG) */
        throw std::runtime_error("Incorrect unterminal expression selected");
#endif /* defined(NDEBUG) */
    }

    void reset_continue_expression_probability()
    {
        expression_depth_                = 0;
        std::uniform_real_distribution<probability_t> dist(0.0, settings_.continue_expression_max_probability);
        continue_expression_probability_ = dist(random_);
    }

    void update_continue_expression_probability()
    {
        if (too_deep_expression())
        {
            continue_expression_probability_ = 0.0;
            return;
        }

        auto&& remaining_depth =
            static_cast<double>(settings_.max_expression_depth - expression_depth_);
        auto&& total_depth     =
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
        auto&& dist = std::bernoulli_distribution{generate_next_statement_probability_};
        return dist(random_);
    }

    bool will_continue_expression()
    {
        auto&& dist = std::bernoulli_distribution{continue_expression_probability_};
        return dist(random_);
    }

    bool enabled(Statement id) const
    {
        assert(id < Statement::STATEMENTS_SIZE);
        return settings_.statements_weights[id] != 0;
    }


    bool enabled(Expression id) const
    {
        assert(id < Expression::EXPRESSIONS_SIZE);
        return settings_.expressions_weights[id] != 0;
    }

    bool enabled(last::node::BinaryOperator::BinaryOperatorT id) const
    {
        assert(id < last::node::BinaryOperator::BINOPS_SIZE);
        return settings_.binary_operators_weights[id] != 0;
    }


    bool enabled(last::node::UnaryOperator::UnaryOperatorT id) const
    {
        assert(id < last::node::UnaryOperator::UNOPS_SIZE);
        return settings_.unary_operators_weights[id] != 0;
    }

    bool too_deep_scope() const
    {
        return scope_depth_ >= settings_.max_scope_depth;
    }

    bool too_deep_expression() const
    {
        return expression_depth_ >= settings_.max_expression_depth;
    }

    bool too_many_statements() const
    {
        return statements_counter_ >= settings_.statements_limit;
    }

    struct NotConstantExpression
    {};

    int calc_constant_expression(last::node::BinaryOperator const & node)
    {
        auto&& type = node.type();

        auto&& right = calc_constant_expression(node.rarg());

        if (node.larg().is_a<last::node::Variable>() and type == last::node::BinaryOperator::ASGN)
            return right;

        auto&& left = calc_constant_expression(node.larg());

        switch (node.type())
        {
            case last::node::BinaryOperator::AND  : return left && right;
            case last::node::BinaryOperator::OR   : return left || right;
            case last::node::BinaryOperator::ADD  : return left +  right;
            case last::node::BinaryOperator::SUB  : return left -  right;
            case last::node::BinaryOperator::MUL  : return left *  right;
            case last::node::BinaryOperator::ISEQ : return left == right;
            case last::node::BinaryOperator::ISNE : return left != right;
            case last::node::BinaryOperator::ISAB : return left >  right;
            case last::node::BinaryOperator::ISABE: return left >= right;
            case last::node::BinaryOperator::ISLS : return left <  right;
            case last::node::BinaryOperator::ISLSE: return left <= right;
            case last::node::BinaryOperator::DIV  : assert(right != 0); return left / right; // expected that right != 0, cause we must already modified it
            case last::node::BinaryOperator::REM  : assert(right != 0); return left % right; // expected that right != 0, cause we must already modified it

            default: break;
        }
        throw NotConstantExpression{};
    }

    int calc_constant_expression(last::node::UnaryOperator const & node)
    {
        auto&& arg = calc_constant_expression(node.arg());
        switch (node.type())
        {
            case last::node::UnaryOperator::PLUS : return +arg;
            case last::node::UnaryOperator::MINUS: return -arg;
            case last::node::UnaryOperator::NOT  : return !arg;
            default: break;
        }

#if defined(NDEBUG)
        __builtin_unreachable();
#else /* defined(NDEBUG) */
        throw std::runtime_error("udnefined unary operator in " + std::string(__PRETTY_FUNCTION__));
#endif /* defined(NDEBUG) */
    }

    int calc_constant_expression(BasicNode const & node)
    {
        if (node.is_a<last::node::BinaryOperator>())
            return calc_constant_expression(static_cast<last::node::BinaryOperator>(node));
        if (node.is_a<last::node::UnaryOperator>())
            return calc_constant_expression(static_cast<last::node::UnaryOperator>(node));
        if (node.is_a<last::node::NumberLiteral>())
            return static_cast<last::node::NumberLiteral>(node).value();

        throw NotConstantExpression{};
    }


    void modify_if_variables_enabled(BasicNode& node)
    {
        /*
            expr
                ->
            ((save_from_zero = expr) == 0 + save_from_zero) (*)

            if expr equal to 0, (*) will be 1, else (*) is equal to expr
        */

        static unique_name_id_t save_from_zero = 0;

        auto&& zero    = last::node::create(last::node::NumberLiteral{0});
        auto&& tmp_var = last::node::create(last::node::Variable{"save_from_zero_" + std::to_string(save_from_zero++)});

        auto&& set_expression_to_variable = last::node::create(last::node::BinaryOperator
        {
            last::node::BinaryOperator::ASGN,
            tmp_var,
            std::move(node)
        });

        auto&& compare_expression_with_null = last::node::create(last::node::BinaryOperator
        {
            last::node::BinaryOperator::ISEQ,
            std::move(set_expression_to_variable),
            std::move(zero)
        });

        node = last::node::create(last::node::BinaryOperator
        {
            last::node::BinaryOperator::ADD,
            std::move(compare_expression_with_null),
            std::move(tmp_var)
        });
    }

    void modify_if_variables_not_enabled_and_value_is_zero(BasicNode& node)
    {
        // this situation is very rare, so we can ignore random generating here :)

        // numbers or ? must be enabled, settings class guaranted it

        if (enabled(Expression::NumberLiteralExpr))
            node = last::node::create(last::node::NumberLiteral{1});
        else
            node = last::node::create(last::node::Scan{});
    }

    void modify_expression_to_guareanted_do_it_not_equal_to_zero(BasicNode& node)
    {
        if (node.is_a<last::node::NumberLiteral>())
        {
            auto&& number = static_cast<last::node::NumberLiteral const &>(node);
            if (number.value() == 0) node = last::node::create(last::node::NumberLiteral{1});
            return;
        }

        if (node.is_a<last::node::Scan>())
            return;

        // if expression is not a constant,
        // calc_constant_expression will throw exception

        try {
        auto&& value = calc_constant_expression(node);
        if (value != 0) return;
        }
        catch(NotConstantExpression) { /* skip */}

        if (enabled(Expression::VariableExpr        ) and
            enabled(Expression::NumberLiteralExpr   ) and
            enabled(Expression::BinaryOperatorExpr  ) and
            enabled(last::node::BinaryOperator::ASGN) and
            enabled(last::node::BinaryOperator::ISEQ) and
            enabled(last::node::BinaryOperator::ADD)
        ) return modify_if_variables_enabled(node);

        return modify_if_variables_not_enabled_and_value_is_zero(node);
    }

private:

    BasicNode generate_scope()
    {
        if (too_deep_scope())
            return generate_not_scoped_statement();

        ++scope_depth_;

        auto&& scope = generate_scope_impl();

        --scope_depth_;

        return scope;
    }

    BasicNode generate_global_scope()
    {
        return generate_scope_impl();
    }

    BasicNode generate_scope_impl()
    {
        name_generator_.new_scope();

        auto&& statements = std::vector<BasicNode>{};
        while ((will_generate_new_statement()) and (not too_many_statements()))
        {
            // auto&& statement = generate_statement();
            // statements.push_back(std::move(statement));
            statements.push_back(generate_statement());
        }

        name_generator_.leave_scope();

        return last::node::create(last::node::Scope{std::move(statements)});
    }

    BasicNode generate_not_scoped_statement()
    {
        auto&& statements = std::vector<Statement>{};
        auto&& weights     = std::vector<weight_t>{};

        auto&& add_expression_if_its_possible = [&](Statement id) -> void
        {
            if (not enabled(id)) return;
            if ((id == Statement::ReturnStmt) and (not inside_function_)) return;

            statements.push_back(id);
            weights.push_back(settings_.statements_weights[id]);
        };

        add_expression_if_its_possible(Statement::AssignStmt);
        add_expression_if_its_possible(Statement::PrintStmt);
        add_expression_if_its_possible(Statement::ExpressionStmt);
        add_expression_if_its_possible(Statement::SemicolonStmt);
        add_expression_if_its_possible(Statement::CommentStmt);
        add_expression_if_its_possible(Statement::ReturnStmt);

        assert(not statements.empty());

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        auto&& chosen = statements[dist(random_)];

        switch (chosen)
        {
            case Statement::AssignStmt    : return generate_assign    ();
            case Statement::PrintStmt     : return generate_print     ();
            case Statement::ExpressionStmt: return generate_expression();
            case Statement::SemicolonStmt : return generate_semicolon ();
            case Statement::CommentStmt   : return generate_comment   ();
            case Statement::ReturnStmt    : return generate_return    ();
            default: break;
        }
#if defined(NDEBUG)
        __builtin_unreachable();
#else /* defined(NDEBUG) */
            throw std::runtime_error("Invalid not scoped statement selected");
#endif /* defined(NDEBUG) */
    }

    BasicNode generate_statement()
    {
        ++statements_counter_;

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

            case Statement::AssignStmt:
                return generate_assign();

            case Statement::PrintStmt:
                return generate_print();

            case Statement::ScopeStmt:
                return generate_scope();

            case Statement::SemicolonStmt:
                return generate_semicolon();

            case Statement::CommentStmt:
                return generate_comment();

            case Statement::ReturnStmt:
                return generate_return();

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
        if (too_deep_scope())
            return generate_not_scoped_statement();

        // needs generate body after generation of tmp var,
        // to in code tmp varis went in otder by their numbers

        auto&& condition = generate_expression();

        if (settings_.guaranteed_to_end_while)
        {
            ++statements_counter_; // for tmp variable declaration

            static unique_name_id_t id = 0;
            auto&& tmp_var = last::node::create(last::node::Variable{"save_from_everlasting_summ__oh_hm_i_mean_while_" + std::to_string(id++)});
            auto&& zero = last::node::create(last::node::NumberLiteral{0});
            auto&& one = last::node::create(last::node::NumberLiteral{1});
            auto&& iteration_limits = last::node::create(last::node::NumberLiteral{static_cast<int>(settings_.while_iterations_limit)});
            auto&& tmp_var_init = last::node::create(last::node::BinaryOperator
                {
                    last::node::BinaryOperator::ASGN,
                    tmp_var,
                    std::move(zero)
                }
            );

            if (too_many_statements())
                return tmp_var_init; // here while is 2, not 1 statement. so we dont want to violate statements limit.

            // auto&& one = last::node::create(last::node::NumberLiteral{1});
            // auto&& iteration_limits = last::node::create(last::node::NumberLiteral{static_cast<int>(settings_.while_iterations_limit)});

            auto&& tmp_var_inc = last::node::create(last::node::BinaryOperator
                {
                    last::node::BinaryOperator::ADDASGN,
                    std::move(tmp_var),
                    std::move(one)
                }
            );

            auto&& limit_check = last::node::create(last::node::BinaryOperator
                {
                    last::node::BinaryOperator::ISLSE,
                    std::move(tmp_var_inc),
                    std::move(iteration_limits)
                }
            );
            condition = last::node::create(last::node::BinaryOperator
                {
                    last::node::BinaryOperator::AND,
                    std::move(condition),
                    std::move(limit_check)
                }
            );

            auto&& body = generate_scope();

            auto&& while_node = last::node::create(last::node::While
                {
                    std::move(condition),
                    std::move(body)
                }
            );

            auto&& init_and_while = last::node::create(last::node::LinearSequence
                {
                    std::move(tmp_var_init),
                    std::move(while_node),
                }
            );

            return init_and_while;
        }

        auto&& body = generate_scope();

        return last::node::create(last::node::While{
            std::move(condition),
            std::move(body)
        });
    }

    BasicNode generate_if()
    {
        if (too_deep_scope())
            return generate_not_scoped_statement();

        auto&& condition = generate_expression();
        auto&& body      = generate_scope();

        auto&& if_node = last::node::create(last::node::If{
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
        // but var was not declared in this statement yet

        auto&& can_generate_function =
            enabled(Expression::FunctionDeclarationExpr) and
            (function_depth_ < settings_.max_function_depth);

        if (can_generate_function && std::bernoulli_distribution(0.3)(random_))
            return generate_function_assign();

        auto&& value        = generate_expression();
        auto&& kind_dist    = std::uniform_int_distribution<int> (0, 1 + (not name_generator_.empty()));
        auto&& kind         = kind_dist(random_);
        auto&& variable     = BasicNode{};

             if (kind == 0) variable = name_generator_.generate_absolute_new_variable();
        else if (kind == 1) variable = name_generator_.generate_new_variable         ();
        else                variable = name_generator_.generate_existing_variable    ();

        return last::node::create(last::node::BinaryOperator
        {
            last::node::BinaryOperator::ASGN,
            std::move(variable),
            std::move(value)
        });
    }

    BasicNode generate_print()
    {
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
        auto&& cut_off_expression = (not will_continue_expression()) or (too_deep_expression());

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
        if (too_deep_expression())
            return generate_terminal_expression();

        ++expression_depth_;

        auto&& must_use_variable = [](last::node::BinaryOperator::BinaryOperatorT id) -> bool
        {
            switch (id)
            {
                case last::node::BinaryOperator::ASGN:
                case last::node::BinaryOperator::ADDASGN:
                case last::node::BinaryOperator::SUBASGN:
                case last::node::BinaryOperator::MULASGN:
                case last::node::BinaryOperator::DIVASGN:
                case last::node::BinaryOperator::REMASGN: return true;
                default: break;
            }
            return false;
        };


        auto&& ops = std::vector<last::node::BinaryOperator::BinaryOperatorT>{};
        auto&& weights = std::vector<weight_t>{};

        auto&& add_binop_if_possible = [&](last::node::BinaryOperator::BinaryOperatorT id) -> void
        {
            auto&& weight = settings_.binary_operators_weights[id];
            if ((weight == 0) or ((not enabled(Expression::VariableExpr)) and must_use_variable(id))) return;

            weights.push_back(weight);
            ops.push_back(id);
        };

        add_binop_if_possible(last::node::BinaryOperator::AND);
        add_binop_if_possible(last::node::BinaryOperator::OR);
        add_binop_if_possible(last::node::BinaryOperator::ADD);
        add_binop_if_possible(last::node::BinaryOperator::SUB);
        add_binop_if_possible(last::node::BinaryOperator::MUL);
        add_binop_if_possible(last::node::BinaryOperator::DIV);
        add_binop_if_possible(last::node::BinaryOperator::REM);
        add_binop_if_possible(last::node::BinaryOperator::ISEQ);
        add_binop_if_possible(last::node::BinaryOperator::ISNE);
        add_binop_if_possible(last::node::BinaryOperator::ISAB);
        add_binop_if_possible(last::node::BinaryOperator::ISABE);
        add_binop_if_possible(last::node::BinaryOperator::ISLS);
        add_binop_if_possible(last::node::BinaryOperator::ISLSE);
        add_binop_if_possible(last::node::BinaryOperator::ADDASGN);
        add_binop_if_possible(last::node::BinaryOperator::SUBASGN);
        add_binop_if_possible(last::node::BinaryOperator::MULASGN);
        add_binop_if_possible(last::node::BinaryOperator::DIVASGN);
        add_binop_if_possible(last::node::BinaryOperator::REMASGN);

#if not defined(NDEBUG)
        if (ops.size() == 0) throw std::runtime_error("no binop enabled, but they was requested");
#endif /* not defined(NDEBUG) */

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        auto&& op = ops[dist(random_)];

        BasicNode lhs;
        BasicNode rhs;

        auto&& op_use_variable_always = (
            op == last::node::BinaryOperator::ADDASGN or
            op == last::node::BinaryOperator::SUBASGN or
            op == last::node::BinaryOperator::MULASGN or
            op == last::node::BinaryOperator::DIVASGN or
            op == last::node::BinaryOperator::REMASGN
        );

        if (op_use_variable_always)
        {
            if (name_generator_.empty())
            {
                --expression_depth_;
                return generate_terminal_expression();
            }

            lhs = generate_variable();

            if (will_continue_expression())
                rhs = generate_expression_impl();
            else
                rhs = generate_terminal_expression();
        }
        else
        {
            if (will_continue_expression())
                lhs = generate_expression_impl();
            else
                lhs = generate_terminal_expression();

            if (will_continue_expression())
                rhs = generate_expression_impl();
            else
                rhs = generate_terminal_expression();
        }

        auto&& rhs_cant_be_zero = ((
            op == last::node::BinaryOperator::DIV or
            op == last::node::BinaryOperator::DIVASGN
            ) and settings_.save_div)
            or ((
                op == last::node::BinaryOperator::REM or
                op == last::node::BinaryOperator::REMASGN
            ) and settings_.save_rem);

        if (rhs_cant_be_zero)
            modify_expression_to_guareanted_do_it_not_equal_to_zero(rhs);

        update_continue_expression_probability();
        --expression_depth_;

        return last::node::create(last::node::BinaryOperator{
            op,
            std::move(lhs),
            std::move(rhs)
        });
    }

    std::string generate_function_name()
    {
        return "fn_" + std::to_string(next_function_id_++);
    }

    std::vector<std::string> generate_function_args() // TODO: потом сделать через отдельный генератор имён
    {
        std::uniform_int_distribution<int> argc_dist(0, 3);
        auto argc = argc_dist(random_);

        std::vector<std::string> args;
        args.reserve(argc);

        for (int i = 0; i < argc; ++i)
            args.push_back("arg_" + std::to_string(i));

        return args;
    }

    BasicNode generate_return()
    {
        if (!inside_function_)
            return generate_expression();

        return last::node::create(last::node::Return{
            generate_expression()
        });
    }

    BasicNode generate_function_assign()
    {
        auto&& value = generate_function_declaration();
        auto&& variable = name_generator_.generate_absolute_new_variable();

        assert(variable.is_a<last::node::Variable>());
        auto&& var = static_cast<last::node::Variable const&>(variable);
        function_names_.push_back(std::string(var.name()));

        return last::node::create(last::node::BinaryOperator
        {
            last::node::BinaryOperator::ASGN,
            std::move(variable),
            std::move(value)
        });
    }

    BasicNode generate_function_declaration()
    {
        if (function_depth_ >= settings_.max_function_depth)
            return generate_terminal_expression();

        ++function_depth_;

        std::vector<std::string> args = {"x"};
        auto body_expr = generate_function_body_expression("x");

        std::vector<BasicNode> body_statements;
        body_statements.push_back(std::move(body_expr));

        auto body = last::node::create(last::node::Scope{std::move(body_statements)});

        --function_depth_;

        return last::node::create(last::node::FunctionDeclaration{
            "",
            std::move(args),
            std::move(body)
        });
    }

    BasicNode generate_function_call()
    {
        if (function_names_.empty())
            return generate_terminal_expression();

        std::uniform_int_distribution<std::size_t> fn_dist(0, function_names_.size() - 1);
        auto fn_name = std::string(function_names_[fn_dist(random_)]);

        std::vector<BasicNode> args;
        args.push_back(generate_terminal_expression());

        return last::node::create(last::node::FunctionCall{
            std::move(fn_name),
            std::move(args)
        });
    }

    BasicNode generate_function_body_expression(std::string const& arg_name)
    {
        auto arg = last::node::create(last::node::Variable{std::string(arg_name)});
        auto num = generate_number();

        std::array<last::node::BinaryOperator::BinaryOperatorT, 4> ops =
        {
            last::node::BinaryOperator::ADD,
            last::node::BinaryOperator::SUB,
            last::node::BinaryOperator::MUL,
            last::node::BinaryOperator::DIV
        };

        std::uniform_int_distribution<std::size_t> op_dist(0, ops.size() - 1);
        auto op = ops[op_dist(random_)];

        BasicNode lhs;
        BasicNode rhs;

        if (std::bernoulli_distribution(0.5)(random_))
        {
            lhs = std::move(arg);
            rhs = std::move(num);
        }
        else
        {
            lhs = std::move(num);
            rhs = last::node::create(last::node::Variable{std::string(arg_name)});
        }

        if (op == last::node::BinaryOperator::DIV)
            modify_expression_to_guareanted_do_it_not_equal_to_zero(rhs);

        return last::node::create(last::node::BinaryOperator{
            op,
            std::move(lhs),
            std::move(rhs)
        });
    }

    BasicNode generate_unary_operator()
    {
        if (too_deep_expression())
            return generate_terminal_expression();

        ++expression_depth_;

        auto&& arg = will_continue_expression() ?
            generate_expression_impl() :
            generate_terminal_expression();

        auto&& ops = std::vector<last::node::UnaryOperator::UnaryOperatorT>{};
        auto&& weights = std::vector<weight_t>{};

        auto&& add_unop_if_possible = [&](last::node::UnaryOperator::UnaryOperatorT id)
        {
            auto&& weight = settings_.unary_operators_weights[id];
            if (weight == 0) return;

            ops.push_back(id);
            weights.push_back(weight);
        };

        add_unop_if_possible(last::node::UnaryOperator::PLUS);
        add_unop_if_possible(last::node::UnaryOperator::MINUS);
        add_unop_if_possible(last::node::UnaryOperator::NOT);

#if not defined(NDEBUG)
        if (ops.size() == 0) throw std::runtime_error("no unop enabled, but they was requested");
#endif /* not defined(NDEBUG) */

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        auto&& op = ops[dist(random_)];

        update_continue_expression_probability();
        --expression_depth_;

        return last::node::create(last::node::UnaryOperator{
            op,
            std::move(arg)
        });
    }

public:
    AstGenerator(SnippySettings const& settings)
        : settings_(settings),
        random_(std::random_device{}()),
        name_generator_(random_),
        generate_next_statement_probability_(settings_.generate_next_statement_probability),
        continue_expression_probability_    (settings_.continue_expression_max_probability),
        делаем_пасхалку_(std::uniform_int_distribution<int>{0, 10}(random_) == 0) {}

public:
    last::AST generate_random_ast()
    {
        auto&& root = generate_global_scope();
        return last::AST(std::move(root));
    }
};

export
last::AST generate_random_ast(SnippySettings const & settings)
{
    return AstGenerator{settings}.generate_random_ast();
}

} /* namespace test_generator */
