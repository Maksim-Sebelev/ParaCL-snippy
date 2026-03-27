module;

#include <unordered_set>
#include <random>
#include <array>

export module ast_builder;

export import thelast;

namespace test_generator
{

using BasicNode = last::node::BasicNode;

export
enum class Statement
{
    ExpressionStmt = 0,
    WhileStmt,
    IfStmt,
    AssignmentStmt,
    STATEMENTS_SIZE
};

export
enum class Expression
{
    AssignmentExpr = 0,
    BinaryOperatorExpr,
    UnaryOperatorExpr,
    InExpr,
    PrintExpr,
    EXPRESSIONS_SIZE
};

export
using probability_t = double;

export
using weight_t = unsigned int;

export
struct AstGeneratorSettings
{
    probability_t generate_next_statement_probability;
    probability_t continue_expression_max_probability;

    std::array<weight_t, STATEMENTS_SIZE> statements_weights;
    std::array<weight_t, EXPRESSIONS_SIZE> expression_weights;
};

export
class AstGenerator
{
public:
    AstGenerator(AstGeneratorSettings && settings);
private:
    
    std::mt19937 random_;

    const probability_t generate_next_statement_probability_;
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

    using generate_stmt_t = BasicNode();

    generate_stmt_t select_random_stmt();

    BasicNode generate_while();
    BasicNode generate_if();
    BasicNode generate_assigment();
    BasicNode generate_print();

    BasicNode generate_expression();
    BasicNode generate_binary_operator()
    BasicNode generate_unary_operator()
    {
        /* ... */

        update_continue_expression_probability();
    }

public:
    last::AST generate_random_ast();
};

} /* namespace test_generator */

