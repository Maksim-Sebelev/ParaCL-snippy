module;

#include <cstddef>
#include <filesystem>
#include <array>

export module settings;

namespace test_generator
{

export
using probability_t = double;

export
using weight_t = unsigned int;

export
enum /* NOT class */ Statement : size_t
{
    ExpressionStmt = 0LU,
    WhileStmt,
    IfStmt,
    PrintStmt,
    AssignStmt,
    ScopeStmt,
    SemicolonStmt,
    CommentStmt,
    STATEMENTS_SIZE
};

export
enum /* NOT class */ Expression : size_t
{
    AssignmentExpr = 0LU,
    BinaryOperatorExpr,
    UnaryOperatorExpr,
    VariableExpr,
    NumberLiteralExpr,
    InExpr,
    PrintExpr,
    EXPRESSIONS_SIZE
};

export
struct SnippySettings
{
    probability_t generate_next_statement_probability = 0.9;
    probability_t continue_expression_max_probability = 0.6;

    std::array<weight_t, Statement ::STATEMENTS_SIZE>  statements_weights;
    std::array<weight_t, Expression::EXPRESSIONS_SIZE> expressions_weights;

    size_t max_scope_depth = 4;
    size_t max_expression_depth = 10;

    bool save_div                : 1 = true;
    bool guaranteed_to_end_while : 1 = true;
    bool bad_tests               : 1 = true;

    size_t while_iterations_limit = 30;

    constexpr SnippySettings()
    {
        // statements weights
        statements_weights[Statement::ExpressionStmt         ] = 1;
        statements_weights[Statement::WhileStmt              ] = 1;
        statements_weights[Statement::IfStmt                 ] = 1;
        statements_weights[Statement::AssignStmt             ] = 1;
        statements_weights[Statement::PrintStmt              ] = 1;
        statements_weights[Statement::ScopeStmt              ] = 1;
        statements_weights[Statement::SemicolonStmt          ] = 1;
        statements_weights[Statement::CommentStmt            ] = 1;

        // expressions weights
        expressions_weights[Expression::AssignmentExpr       ] = 1;
        expressions_weights[Expression::BinaryOperatorExpr   ] = 1;
        expressions_weights[Expression::UnaryOperatorExpr    ] = 1;
        expressions_weights[Expression::VariableExpr         ] = 1;
        expressions_weights[Expression::NumberLiteralExpr    ] = 1;
        expressions_weights[Expression::InExpr               ] = 0;
        expressions_weights[Expression::PrintExpr            ] = 1;
    }

    void check_configuration() const
    {
        auto&& has_statement_weight = false;
        for (auto&& w : statements_weights)
        {
            if (w == 0) continue;
            has_statement_weight = true;
            break;
        }

        if (!has_statement_weight)
            throw std::runtime_error("All statement weights are zero. Incorrect generator configuration.");

        auto&& has_expression_weight = false;
        for (auto&& w : expressions_weights)
        {
            if (w == 0) continue;
            has_expression_weight = true;
            break;
        }

        if (!has_expression_weight)
            throw std::runtime_error("All expression weights are zero. Incorrect generator configuration.");

        auto&& has_terminal_expression =
            expressions_weights[Expression::NumberLiteralExpr] != 0 or
            expressions_weights[Expression::InExpr           ] != 0;

        if (not has_terminal_expression)
            throw std::runtime_error(
                "Both VariableExpr and NumberLiteralExpr weights are zero.\n"
                "Terminal expressions are disabled, so the generator is incorrectly configured.\n"
                "Also, only enabled Variables are useless, cause it`s impossible to use only variables, without other terminal expressions."
            );
    }
};


} /* namespace test_generator */