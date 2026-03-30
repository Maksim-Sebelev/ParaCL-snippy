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
    VariableDeclarationStmt,
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
    probability_t continue_expression_max_probability = 0.8;

    std::array<weight_t, Statement ::STATEMENTS_SIZE>  statements_weights;
    std::array<weight_t, Expression::EXPRESSIONS_SIZE> expressions_weights;

    size_t max_scope_depth = 7;
    size_t max_expression_depth = 100;

    bool save_div                : 1  = true;
    bool guaranteed_to_end_while : 1  = true;

    size_t while_iterations_limit = 100;

    constexpr SnippySettings()
    {
        // statements weights
        statements_weights[Statement::ExpressionStmt         ] = 1;
        statements_weights[Statement::WhileStmt              ] = 1;
        statements_weights[Statement::IfStmt                 ] = 1;
        statements_weights[Statement::VariableDeclarationStmt] = 1;
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
        expressions_weights[Expression::InExpr               ] = 1;
        expressions_weights[Expression::PrintExpr            ] = 1;
    }
};


} /* namespace test_generator */