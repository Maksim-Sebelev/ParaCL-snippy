module;

#include <stdexcept>
#include <cstddef>
#include <filesystem>
#include <array>
#include <iostream>

export module settings;

import thelast;

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
    ReturnStmt,
    STATEMENTS_SIZE
};

export
enum /* NOT class */ Expression : size_t
{
    BinaryOperatorExpr = 0LU,
    UnaryOperatorExpr,
    VariableExpr,
    NumberLiteralExpr,
    InExpr,
    PrintExpr,
    FunctionDeclarationExpr,
    FunctionCallExpr,
    EXPRESSIONS_SIZE
};


export
struct SnippySettings
{
private:
    using BinaryOperator = last::node::BinaryOperator::BinaryOperatorT;
    using UnaryOperator  = last::node::UnaryOperator::UnaryOperatorT;

public:
    probability_t generate_next_statement_probability = 0.8;
    probability_t continue_expression_max_probability = 0.6;

    std::array<weight_t, Statement ::STATEMENTS_SIZE>  statements_weights;
    std::array<weight_t, Expression::EXPRESSIONS_SIZE> expressions_weights;
    std::array<weight_t, BinaryOperator::BINOPS_SIZE>  binary_operators_weights;
    std::array<weight_t, UnaryOperator :: UNOPS_SIZE>  unary_operators_weights;


    size_t statements_limit = 1000;

    std::size_t max_scope_depth      = 4;
    std::size_t max_expression_depth = 10;
    std::size_t max_function_depth_  = 2;

    bool save_div                : 1 = true;
    bool save_rem                : 1 = true;
    bool guaranteed_to_end_while : 1 = true;

    size_t while_iterations_limit = 30;

    constexpr SnippySettings()
    {
        auto&& counter = 0LU;
        // statements weights
        statements_weights[Statement::ExpressionStmt         ] = 1; ++counter;
        statements_weights[Statement::WhileStmt              ] = 1; ++counter;
        statements_weights[Statement::IfStmt                 ] = 1; ++counter;
        statements_weights[Statement::AssignStmt             ] = 1; ++counter;
        statements_weights[Statement::PrintStmt              ] = 1; ++counter;
        statements_weights[Statement::ScopeStmt              ] = 1; ++counter;
        statements_weights[Statement::SemicolonStmt          ] = 1; ++counter;
        statements_weights[Statement::CommentStmt            ] = 1; ++counter;
        statements_weights[Statement::ReturnStmt             ] = 1; ++counter;

        if (counter != Statement::STATEMENTS_SIZE) throw std::runtime_error("Not all statements was default initialized");
        counter = 0LU;

        // expressions weights
        expressions_weights[Expression::BinaryOperatorExpr     ] = 1; ++counter;
        expressions_weights[Expression::UnaryOperatorExpr      ] = 1; ++counter;
        expressions_weights[Expression::VariableExpr           ] = 1; ++counter;
        expressions_weights[Expression::NumberLiteralExpr      ] = 1; ++counter;
        expressions_weights[Expression::InExpr                 ] = 0; ++counter;
        expressions_weights[Expression::PrintExpr              ] = 1; ++counter;
        expressions_weights[Expression::FunctionDeclarationExpr] = 1; ++counter;
        expressions_weights[Expression::FunctionCallExpr       ] = 1; ++counter;

        if (counter != Expression::EXPRESSIONS_SIZE) throw std::runtime_error("Not all expressions was default initialized");
        counter = 0LU;

        // binary operators weights
        binary_operators_weights[BinaryOperator::AND         ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::OR          ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::ADD         ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::SUB         ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::MUL         ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::DIV         ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::REM         ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::ASGN        ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::ADDASGN     ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::SUBASGN     ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::MULASGN     ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::DIVASGN     ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::REMASGN     ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::ISEQ        ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::ISNE        ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::ISAB        ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::ISABE       ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::ISLS        ] = 1; ++counter;
        binary_operators_weights[BinaryOperator::ISLSE       ] = 1; ++counter;

        if (counter != BinaryOperator::BINOPS_SIZE) throw std::runtime_error("Not all binary operators was default initialized");
        counter = 0LU;

        // unary operators weights
        unary_operators_weights[UnaryOperator::MINUS         ] = 1; ++counter;
        unary_operators_weights[UnaryOperator::PLUS          ] = 1; ++counter;
        unary_operators_weights[UnaryOperator::NOT           ] = 1; ++counter;

        if (counter != UnaryOperator::UNOPS_SIZE) throw std::runtime_error("Not all unary operators was default initialized");
        counter = 0LU;
    }

private:
    void check_statements_weight() const
    {
        auto&& has_statement_weight = false;
        for (auto&& w : statements_weights)
        {
            if (w == 0) continue;
            has_statement_weight = true;
            break;
        }

        if (has_statement_weight) return;
        throw std::runtime_error("All statement weights are zero. Incorrect generator configuration.");
    }

    void check_expressions_weight() const
    {
        auto&& has_expression_weight = false;
        for (auto&& w : expressions_weights)
        {
            if (w == 0) continue;
            has_expression_weight = true;
            break;
        }

        if (!has_expression_weight)
            throw std::runtime_error("All expression weights are zero. Incorrect generator configuration.");

    }

    void check_binop_weight() const
    {
        if (expressions_weights[Expression::BinaryOperatorExpr] == 0) return;

        auto&& has_binop_weiht = false;
        for (auto&& w : binary_operators_weights)
        {
            if (w == 0) continue;
            has_binop_weiht = true;
            break;
        }
        if (has_binop_weiht) return;

        throw std::runtime_error("All binary operatos weights are zero, but they are using in expressions. Incorrect generator configuration.");
    }

    void check_unop_weight() const
    {
        if (expressions_weights[Expression::UnaryOperatorExpr] == 0) return;

        auto&& has_unop_weiht = false;
        for (auto&& w : unary_operators_weights)
        {
            if (w == 0) continue;
            has_unop_weiht = true;
            break;
        }
        if (has_unop_weiht) return;

        throw std::runtime_error("All unary operatos weights are zero, but they are using in expressions. Incorrect generator configuration.");
    }


    void check_terminal_expressions() const
    {
        auto&& has_terminal_expression =
            expressions_weights[Expression::NumberLiteralExpr] != 0 or
            expressions_weights[Expression::InExpr           ] != 0;

        if (has_terminal_expression) return;

        throw std::runtime_error(
            "NumberLiteralExpr and InExpr weights are zero.\n"
            "Terminal expressions are disabled, so the generator is incorrectly configured.\n"
            "Also, only enabled Variables are useless, cause it`s impossible to use only variables, without other terminal expressions."
        );
    }

    void check_terminal_statements() const
    {
        auto&& has_terminal_statements = (
            statements_weights[Statement::ExpressionStmt] != 0 or
            statements_weights[Statement::PrintStmt     ] != 0 or
            statements_weights[Statement::AssignStmt    ] != 0 or
            statements_weights[Statement::SemicolonStmt ] != 0 or
            statements_weights[Statement::CommentStmt   ] != 0
        );

        if (has_terminal_statements) return;

        throw std::runtime_error("No terminal statements enabled (print, expressions, ...). Incorrect generator configuration");
    }

public:
    void check_configuration() const
    {
        check_statements_weight();
        check_terminal_statements();
        check_expressions_weight();
        check_binop_weight();
        check_unop_weight();
        check_terminal_expressions();
    }
};


} /* namespace test_generator */