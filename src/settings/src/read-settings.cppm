module;

#include <cstdint>
#include <array>
#include <string>
#include <string_view>
#include <stdexcept>
#include <filesystem>
#include <cassert>
#include <fstream>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <iostream>

export module test_generator_settings_reader;

export import test_generator_settings;

namespace test_generator
{

weight_t extract_weight(const boost::json::value& val, const std::string& field_name)
{
    if (val.is_int64())
    {
        auto&& v = val.as_int64();
        if (v >= 0) return static_cast<weight_t>(v);

        throw std::runtime_error("Field '" + field_name + "' must be positive integer, but got " +
                                    std::to_string(v));
    }

    if (val.is_uint64())
    {
        return static_cast<weight_t>(val.as_uint64());
    }

    throw std::runtime_error("Field '" + field_name + "' has unsupported type. It must be a positive integer");
}

void read_statements_weights(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    if (not root_obj.contains("statements-weights"))
        return;

    auto&& statements_weights_obj = root_obj.at("statements-weights");

    if (not statements_weights_obj.is_object())
        throw std::runtime_error("Field 'statements-weights' must be an object");

    auto&& weights = statements_weights_obj.as_object();

    auto&& expression_name = "expression";
    if (auto val = weights.if_contains(expression_name))
        settings.statements_weights[Statement::ExpressionStmt] = extract_weight(*val, expression_name);

    auto&& while_name = "while";
    if (auto val = weights.if_contains(while_name))
        settings.statements_weights[Statement::WhileStmt] = extract_weight(*val, while_name);

    auto&& if_name = "if";
    if (auto val = weights.if_contains(if_name))
        settings.statements_weights[Statement::IfStmt] = extract_weight(*val, if_name);

    auto&& assign_name = "assign";
    if (auto val = weights.if_contains(assign_name))
        settings.statements_weights[Statement::AssignmentStmt] = extract_weight(*val, assign_name);

    // auto&& in_name = "in";
    // if (auto val = weights.if_contains(in_name))
    //     settings.statements_weights[Statement::InStmt] = extract_weight(*val, in_name);

    auto&& print_name = "print";
    if (auto val = weights.if_contains(print_name))
        settings.statements_weights[Statement::PrintStmt] = extract_weight(*val, print_name);
}

void read_expressions_weights(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    if (not root_obj.contains("expressions-weights"))
        return;

    auto&& expressions_weights_obj = root_obj.at("expressions-weights");

    if (not expressions_weights_obj.is_object())
        throw std::runtime_error("Field 'expressions-weights' must be an object");

    auto&& weights = expressions_weights_obj.as_object();

    auto&& binop_name = "binop";
    if (auto val = weights.if_contains(binop_name))
        settings.expressions_weights[Expression::BinaryOperatorExpr] = extract_weight(*val, binop_name);

    auto&& unop_name = "unop";
    if (auto val = weights.if_contains(unop_name))
        settings.expressions_weights[Expression::UnaryOperatorExpr] = extract_weight(*val, unop_name);

    auto&& in_name = "in";
    if (auto val = weights.if_contains(in_name))
        settings.expressions_weights[Expression::InExpr] = extract_weight(*val, in_name);

    auto&& print_name = "print";
    if (auto val = weights.if_contains(print_name))
        settings.expressions_weights[Expression::PrintExpr] = extract_weight(*val, print_name);
}

void read_generate_next_statement_probability(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    auto&& key = "next-stmt-p";
    if (not root_obj.contains(key))
        return;

    auto&& next_stmt_p_val = root_obj.at(key);

    if (next_stmt_p_val.is_double())
    {
        auto&& value = next_stmt_p_val.as_double();
        if ((0.0 <= value) and(value <= 1.0))
        {
            settings.generate_next_statement_probability = static_cast<probability_t>(value);
            return;
        }
        throw std::runtime_error("Field '" + std::string(key) + "' must be a floating point number between 0 and 1.");
    }

    if (next_stmt_p_val.is_int64())
    {
        auto&& value = next_stmt_p_val.as_int64();
        if ((value == 0) or (value == 1))
        {
            settings.generate_next_statement_probability = static_cast<probability_t>(value);
            return;
        }
        throw std::runtime_error("Field '" + std::string(key) + "' must be 0, 1, or a floating point number between 0 and 1.");
    }

    if (next_stmt_p_val.is_uint64())
    {
        auto&& value = next_stmt_p_val.as_uint64();
        if ((value == 0) or (value == 1))
        {
            settings.generate_next_statement_probability = static_cast<probability_t>(value);
            return;
        }
        throw std::runtime_error("Field '" + std::string(key) + "' must be 0, 1, or a floating point number between 0 and 1.");
    }

    throw std::runtime_error("Field '" + std::string(key) + "' must be a number (0, 1, or between 0 and 1).");
}

void read_continue_expression_max_probability(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    auto&& key = "continue-expr-max-p";
    if (not root_obj.contains(key)) return;

    auto&& next_stmt_p_val = root_obj.at(key);

    if (next_stmt_p_val.is_double())
    {
        auto&& value = next_stmt_p_val.as_double();
        if ((0.0 <= value) and (value <= 1.0))
        {
            settings.continue_expression_max_probability = static_cast<probability_t>(value);
            return;
        }
        throw std::runtime_error("Field '" + std::string(key) + "' must be a floating point number between 0 and 1.");
    }

    if (next_stmt_p_val.is_int64())
    {
        auto&& value = next_stmt_p_val.as_int64();
        if ((value == 0) or (value == 1))
        {
            settings.continue_expression_max_probability = static_cast<probability_t>(value);
            return;
        }
        throw std::runtime_error("Field '" + std::string(key) + "' must be 0, 1, or a floating point number between 0 and 1.");
    }

    if (next_stmt_p_val.is_uint64())
    {
        auto&& value = next_stmt_p_val.as_uint64();
        if ((value == 0) or (value == 1))
        {
            settings.continue_expression_max_probability = static_cast<probability_t>(value);
            return;
        }
        throw std::runtime_error("Field '" + std::string(key) + "' must be 0, 1, or a floating point number between 0 and 1.");
    }

    throw std::runtime_error("Field '" + std::string(key) + "' must be a number (0, 1, or between 0 and 1).");
}


SnippySettings read_settings_(std::string_view json_data)
{
    auto&& jv = boost::json::parse(json_data);

    auto&& settings = SnippySettings{};
    read_statements_weights(jv, settings);
    read_expressions_weights(jv, settings);
    read_generate_next_statement_probability(jv, settings);
    read_continue_expression_max_probability(jv, settings);
    return settings;
}

export
SnippySettings read_settings(std::filesystem::path const & setting_json)
{
    auto ifs = std::ifstream{setting_json};

    if (ifs.fail())
        throw std::runtime_error("No such file: `"+setting_json.string()+"`");

    auto buffer = std::ostringstream{};
    buffer << ifs.rdbuf();
    auto json_data = buffer.str();

    return read_settings_(json_data);
}

} /* namespace test_generator */