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
    AssignmentStmt,
    STATEMENTS_SIZE
};

export
enum /* NOT class */ Expression : size_t
{
    AssignmentExpr = 0LU,
    BinaryOperatorExpr,
    UnaryOperatorExpr,
    InExpr,
    PrintExpr,
    EXPRESSIONS_SIZE
};

export
struct SnippySettings
{
    probability_t generate_next_statement_probability;
    probability_t continue_expression_max_probability;

    std::array<weight_t, Statement::STATEMENTS_SIZE> statements_weights;
    std::array<weight_t, Expression::EXPRESSIONS_SIZE> expression_weights;

    std::filesystem::path output_file;
};

} /* namespace test_generator */
