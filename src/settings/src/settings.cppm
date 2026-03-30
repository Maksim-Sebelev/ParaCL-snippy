module;

#include <cstddef>
#include <filesystem>
#include <array>

export module test_generator_settings;

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

    std::filesystem::path output_file = "a.cl";

    size_t max_scope_depth = 7;
    size_t max_expression_depth = 100;

    SnippySettings()
    {
        // statements weights
        statements_weights[Statement::ExpressionStmt         ] = 1;
        statements_weights[Statement::WhileStmt              ] = 1;
        statements_weights[Statement::IfStmt                 ] = 1;
        statements_weights[Statement::VariableDeclarationStmt] = 1;
        statements_weights[Statement::PrintStmt              ] = 1;
        statements_weights[Statement::ScopeStmt              ] = 1;
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