module;

#include <cstddef>
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

export module settings_reader;

export import settings;

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

    auto&& parse = [&](Statement id, std::string name) -> void
    {
        if (auto&& val = weights.if_contains(name))
            settings.statements_weights[id] = extract_weight(*val, name);
    };

    parse(Statement::ExpressionStmt         , "expression" );
    parse(Statement::WhileStmt              , "while"      );
    parse(Statement::IfStmt                 , "if"         );
    parse(Statement::AssignStmt, "declaration");
    parse(Statement::PrintStmt              , "print"      );
    parse(Statement::ScopeStmt              , "scope"      );
    parse(Statement::CommentStmt            , "comment"    );
    parse(Statement::SemicolonStmt          , "semicolon"  );
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
    auto&& parse = [&](Expression id, std::string name) -> void
    {
        if (auto&& val = weights.if_contains(name))
            settings.expressions_weights[id] = extract_weight(*val, name);
    };

    parse(Expression::BinaryOperatorExpr, "binop"   );
    parse(Expression::UnaryOperatorExpr , "unop"    );
    parse(Expression::InExpr            , "in"      );
    parse(Expression::PrintExpr         , "print"   );
    parse(Expression::VariableExpr      , "variable");
    parse(Expression::NumberLiteralExpr , "number"  );
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
    }
    throw std::runtime_error("Field '" + std::string(key) + "' must be 0, 1, or a floating point number between 0 and 1.");
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
    }
    throw std::runtime_error("Field '" + std::string(key) + "' must be 0, 1, or a floating point number between 0 and 1.");
}

void read_max_statement_depth(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    auto&& key = "max-scope-depth";
    if (not root_obj.contains(key)) return;

    auto&& json_val = root_obj.at(key);

    if (json_val.is_int64())
    {
        auto&& value = json_val.as_int64();
        if (value >= 0)
        {
            settings.max_scope_depth = static_cast<size_t>(value);
            return;
        }
    }
    throw std::runtime_error("Field '" + std::string(key) + "' must non negative integer.");
}

void read_max_expression_depth(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    auto&& key = "max-expression-depth";
    if (not root_obj.contains(key)) return;

    auto&& json_value = root_obj.at(key);

    if (json_value.is_int64())
    {
        auto&& value = json_value.as_int64();
        if (value >= 0)
        {
            settings.max_expression_depth = static_cast<size_t>(value);
            return;
        }
    }
    throw std::runtime_error("Field '" + std::string(key) + "' must non negative integer.");
}

void read_save_div(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    auto&& key = "save-div";
    if (not root_obj.contains(key)) return;

    auto&& json_value = root_obj.at(key);

    if (json_value.is_bool())
    {
        auto&& value = json_value.as_bool();
        settings.save_div = static_cast<bool>(value);
        return;
    }
    throw std::runtime_error("Field '" + std::string(key) + "' must be bool.");
}

void read_save_while(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    auto&& key = "save-while";
    if (not root_obj.contains(key)) return;

    auto&& json_value = root_obj.at(key);

    if (json_value.is_bool())
    {
        auto&& value = json_value.as_bool();
        settings.guaranteed_to_end_while = static_cast<bool>(value);
        return;
    }
    throw std::runtime_error("Field '" + std::string(key) + "' must non negative integer.");
}

void read_while_iterations_limit(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    auto&& key = "while-iterations-limit";
    if (not root_obj.contains(key)) return;

    auto&& json_value = root_obj.at(key);

    if (json_value.is_int64())
    {
        auto&& value = json_value.as_int64();
        if (value >= 0)
        {
            settings.while_iterations_limit = static_cast<size_t>(value);
            return;
        }
        return;
    }
    throw std::runtime_error("Field '" + std::string(key) + "' must be bool.");
}


SnippySettings read_settings_(std::string_view json_data)
{
    auto&& jv = boost::json::parse(json_data);

    auto&& settings = SnippySettings{};

    read_statements_weights(jv, settings);
    read_expressions_weights(jv, settings);
    read_max_statement_depth(jv, settings);
    read_max_expression_depth(jv, settings);
    read_generate_next_statement_probability(jv, settings);
    read_continue_expression_max_probability(jv, settings);
    read_save_div(jv, settings);
    read_save_while(jv, settings);
    read_while_iterations_limit(jv, settings);

    return settings;
}

export
SnippySettings read_settings(std::filesystem::path const & setting_json)
{
    auto&& ifs = std::ifstream{setting_json};

    if (ifs.fail()) return SnippySettings{}; // if no file specialized, using default settings

    auto&& buffer = std::ostringstream{};
    buffer << ifs.rdbuf();
    auto&& json_data = buffer.str();

    return read_settings_(json_data);
}

} /* namespace test_generator */
