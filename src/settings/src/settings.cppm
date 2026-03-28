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
    PrintStm,
    InStm,
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
    probability_t generate_next_statement_probability = 0.3;
    probability_t continue_expression_max_probability = 0.2;

    std::array<weight_t, Statement::STATEMENTS_SIZE> statements_weights   = {1,1,1,1,1,1};
    std::array<weight_t, Expression::EXPRESSIONS_SIZE> expression_weights = {1,1,1,1,1};

    std::filesystem::path output_file = "a.cl";
};

} /* namespace test_generator */
