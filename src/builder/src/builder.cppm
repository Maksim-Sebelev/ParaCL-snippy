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

    std::size_t statement_depth_  = 0LU;
    std::size_t expression_depth_ = 0LU;

    bool делаем_пасхалку_;
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

    void update_continue_expression_probability()
    {
        if (expression_depth_ >= settings_.max_expression_depth)
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

private:

    BasicNode generate_scope()
    {
        if (statement_depth_ >= settings_.max_scope_depth)
            return generate_not_scoped_statement();

        ++statement_depth_;

        auto&& scope = generate_scope_impl();

        --statement_depth_;

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
        do
        {
            statements.push_back(generate_statement());
        }
        while (will_generate_new_statement());

        name_generator_.leave_scope();

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

        add_expression_if_its_possible(Statement::AssignStmt);
        add_expression_if_its_possible(Statement::PrintStmt);
        add_expression_if_its_possible(Statement::ExpressionStmt);
        add_expression_if_its_possible(Statement::SemicolonStmt);
        add_expression_if_its_possible(Statement::CommentStmt);

        if (statements.empty())
            throw std::runtime_error("Requests not scoped statement, but no one is enabled");

        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        auto&& chosen = statements[dist(random_)];

        switch (chosen)
        {
            case Statement::AssignStmt:
                return generate_statement();

            case Statement::PrintStmt:
                return generate_print();

            case Statement::ExpressionStmt:
                return generate_expression();

            case Statement::SemicolonStmt:
                return generate_semicolon();

            case Statement::CommentStmt:
                return generate_comment();

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
    void modify_expression_to_guareanted_do_it_not_equal_to_zero(BasicNode& node)
    {
        if (node.is_a<last::node::NumberLiteral>())
        {
            auto&& number = static_cast<last::node::NumberLiteral const &>(node);
            if (number.value() == 0) node = last::node::create(last::node::NumberLiteral{1});
            return;
        }
        static unique_name_id_t save_from_zero = 0;

        /*
            something / expr
                ->
            something / ((save_div_tmp_var = expr) == 0 + save_div_tmp_var)
        */

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

    BasicNode generate_while()
    {
        if (statement_depth_ >= settings_.max_scope_depth)
            return generate_not_scoped_statement();

        // needs generate body after generation of tmp var,
        // to in code tmp varis went in otder by their numbers

        auto&& condition = generate_expression();

        if (settings_.guaranteed_to_end_while)
        {
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
        if (statement_depth_ >= settings_.max_scope_depth)
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
            if ((weight == 0) or ((settings_.expressions_weights[Expression::VariableExpr] == 0) and must_use_variable(id))) return;

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

    BasicNode generate_unary_operator()
    {
        if (expression_depth_ >= settings_.max_expression_depth)
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
        делаем_пасхалку_(std::uniform_int_distribution<int>{0, 10}(random_) == 0)
    {
    }

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
