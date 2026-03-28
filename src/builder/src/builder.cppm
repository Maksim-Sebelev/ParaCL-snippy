module;

#include <cstdlib>
#include <random>

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
public:
    AstGenerator(SnippySettings const & settings) :
        settings_(settings), name_generator_(random_)
    {}

private:
    SnippySettings settings_;
    std::mt19937         random_;
    NameGenerator        name_generator_;

    probability_t generate_next_statement_probability_;
    probability_t continue_expression_probability_;

    void update_continue_expression_probability()
    {
        continue_expression_probability_ = /* ? */ 0;
    }

    bool will_generate_new_statement()
    {
        auto&& ans = std::bernoulli_distribution{generate_next_statement_probability_};
        return ans(random_);
    }

    bool will_continue_expression()
    {
        auto&& ans = std::bernoulli_distribution{continue_expression_probability_};
        return ans(random_);
    }

private:
    BasicNode generate_statement()
    {
        /*
        while (will_generate_new_statement)
        {
            auto&& generatror select_random_stmt();
            node = generatror();
        }
        */
    }

    BasicNode generate_while();
    BasicNode generate_if();
    BasicNode generate_assigment();
    BasicNode generate_print();

    BasicNode generate_expression();
    BasicNode generate_binary_operator();
    BasicNode generate_unary_operator()
    {
        /* ... */

        update_continue_expression_probability();
    }

public:
    last::AST generate_random_ast() { return last::AST{}; }
};

export
last::AST generate_random_ast(SnippySettings const & settings)
{
    return AstGenerator{settings}.generate_random_ast();
}

} /* namespace test_generator */

