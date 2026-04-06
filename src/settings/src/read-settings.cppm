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

import thelast;

namespace test_generator
{

weight_t extract_weight(const boost::json::value& val, const std::string& field_name)
{
    if (val.is_int64())
    {
        auto&& v = val.as_int64();
        if (v >= 0) return static_cast<weight_t>(v);
    }

    throw std::runtime_error("Field '" + field_name + "' must be positive integer");
}

void read_probability_of(boost::json::value const & jv, std::string_view key, probability_t& probability)
{
    auto&& root_obj = jv.as_object();

    if (not root_obj.contains(key)) return;

    auto&& obj = root_obj.at(key);

    if (obj.is_double())
    {
        auto&& value = obj.as_double();
        if ((0.0 <= value) and (value <= 1.0))
        {
            probability = static_cast<probability_t>(value);
            return;
        }
    }

    if (obj.is_int64())
    {
        auto&& value = obj.as_int64();
        if ((value == 0) or (value == 1))
        {
            probability = static_cast<probability_t>(value);
            return;
        }
    }
    throw std::runtime_error("Field '" + std::string(key) + "' must be 0, 1, or a floating point number between 0 and 1.");
}

void read_limit_of(boost::json::value const & jv, std::string_view key, size_t& limit)
{
    auto&& root_obj = jv.as_object();

    if (not root_obj.contains(key)) return;

    auto&& json_val = root_obj.at(key);

    if (json_val.is_int64())
    {
        auto&& value = json_val.as_int64();
        if (value >= 0)
        {
            limit = static_cast<size_t>(value);
            return;
        }
    }
    throw std::runtime_error("Field '" + std::string(key) + "' must non negative integer.");
}

void read_flag_of(boost::json::value const & jv, std::string_view key, bool& flag)
{
    auto&& root_obj = jv.as_object();

    if (not root_obj.contains(key)) return;

    auto&& json_value = root_obj.at(key);

    if (json_value.is_bool())
    {
        auto&& value = json_value.as_bool();
        flag = static_cast<bool>(value);
        return;
    }

    throw std::runtime_error("Field '" + std::string(key) + "' must be bool.");
}

void read_statements_weights(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();

    auto&& key = "statements-weights";

    if (not root_obj.contains(key))
        return;

    auto&& statements_weights_obj = root_obj.at(key);

    if (not statements_weights_obj.is_object())
        throw std::runtime_error("Field 'statements-weights' must be an object");

    auto&& weights = statements_weights_obj.as_object();

    auto&& parse = [&](Statement id, std::string name) -> void
    {
        if (auto&& val = weights.if_contains(name))
            settings.statements_weights[id] = extract_weight(*val, name);
    };

    parse(Statement::ExpressionStmt, "expression" );
    parse(Statement::WhileStmt     , "while"      );
    parse(Statement::IfStmt        , "if"         );
    parse(Statement::PrintStmt     , "print"      );
    parse(Statement::AssignStmt    , "assign"     );
    parse(Statement::ScopeStmt     , "scope"      );
    parse(Statement::CommentStmt   , "comment"    );
    parse(Statement::SemicolonStmt , "semicolon"  );
}

void read_binop_weights(boost::json::object const & jo, SnippySettings& settings)
{
    using BinaryOperator = last::node::BinaryOperator::BinaryOperatorT;

    auto&& key = "binop-weights";
    if (not jo.contains(key))
        return;

    auto&& expressions_weights_obj = jo.at(key);

    if (not expressions_weights_obj.is_object())
        throw std::runtime_error("Field 'binop-weights' must be an object");

    auto&& weights = expressions_weights_obj.as_object();
    auto&& parse = [&](BinaryOperator id, std::string name) -> void
    {
        if (auto&& val = weights.if_contains(name))
            settings.binary_operators_weights[id] = extract_weight(*val, name);
    };

    parse(BinaryOperator::AND    , "and"    );
    parse(BinaryOperator::OR     , "or"     );
    parse(BinaryOperator::ADD    , "add"    );
    parse(BinaryOperator::SUB    , "sub"    );
    parse(BinaryOperator::MUL    , "mul"    );
    parse(BinaryOperator::DIV    , "div"    );
    parse(BinaryOperator::REM    , "rem"    );
    parse(BinaryOperator::ASGN   , "asgn"   );
    parse(BinaryOperator::ADDASGN, "addasgn");
    parse(BinaryOperator::SUBASGN, "subasgn");
    parse(BinaryOperator::MULASGN, "mulasgn");
    parse(BinaryOperator::DIVASGN, "divasgn");
    parse(BinaryOperator::REMASGN, "remasgn");
    parse(BinaryOperator::ISEQ   , "iseq"   );
    parse(BinaryOperator::ISNE   , "isne"   );
    parse(BinaryOperator::ISAB   , "isab"   );
    parse(BinaryOperator::ISABE  , "isabe"  );
    parse(BinaryOperator::ISLS   , "isls"   );
    parse(BinaryOperator::ISLSE  , "islse"  );
}


void read_unop_weights(boost::json::object const & jo, SnippySettings& settings)
{
    using UnaryOperator = last::node::UnaryOperator::UnaryOperatorT;

    auto&& key = "unop-weights";
    if (not jo.contains(key)) return;

    auto&& expressions_weights_obj = jo.at(key);

    if (not expressions_weights_obj.is_object())
        throw std::runtime_error("Field 'unop-weights' must be an object");

    auto&& weights = expressions_weights_obj.as_object();
    auto&& parse = [&](UnaryOperator id, std::string name) -> void
    {
        if (auto&& val = weights.if_contains(name))
            settings.unary_operators_weights[id] = extract_weight(*val, name);
    };

    parse(UnaryOperator::PLUS , "plus" );
    parse(UnaryOperator::MINUS, "minus");
    parse(UnaryOperator::NOT  , "not"  );
}

void read_expressions_weights(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& root_obj = jv.as_object();
    auto&& key = "expressions-weights";

    if (not root_obj.contains(key))
        return;

    auto&& expressions_weights_obj = root_obj.at(key);

    if (not expressions_weights_obj.is_object())
        throw std::runtime_error("Field 'expressions-weights' must be an object");

    auto&& weights = expressions_weights_obj.as_object();
    auto&& parse = [&](Expression id, std::string name) -> void
    {
        if (auto&& val = weights.if_contains(name))
            settings.expressions_weights[id] = extract_weight(*val, name);
    };

    parse(Expression::BinaryOperatorExpr      , "binop"   );
    parse(Expression::UnaryOperatorExpr       , "unop"    );
    parse(Expression::InExpr                  , "in"      );
    parse(Expression::PrintExpr               , "print"   );
    parse(Expression::VariableExpr            , "variable");
    parse(Expression::NumberLiteralExpr       , "number"  );
    parse(Expression::FunctionCallExpr        , "funccall");
    parse(Expression::FunctionDeclarationExpr , "funcdecl");

    read_binop_weights(weights, settings);
    read_unop_weights (weights, settings);
}

void read_generate_next_statement_probability(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& key = "next-stmt-p";
    read_probability_of(jv, key, settings.generate_next_statement_probability);
}

void read_continue_expression_max_probability(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& key = "continue-expr-max-p";
    read_probability_of(jv, key, settings.continue_expression_max_probability);
}

void read_max_statement_depth(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& key = "max-scope-depth";
    read_limit_of(jv, key, settings.max_scope_depth);
}

void read_statements_limit(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& key = "statements-limit";
    read_limit_of(jv, key, settings.statements_limit);
}

void read_max_expression_depth(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& key = "max-expression-depth";
    read_limit_of(jv, key, settings.max_expression_depth);
}

void read_save_div(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& key = "save-div";
    auto&& flag = static_cast<bool>(settings.save_div);
    read_flag_of(jv, key, flag);
    settings.save_div = flag;
}

void read_save_rem(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& key = "save-rem";
    auto&& flag = static_cast<bool>(settings.save_rem);
    read_flag_of(jv, key, flag);
    settings.save_rem = flag;
}

void read_save_while(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& key = "save-while";
    auto&& flag = static_cast<bool>(settings.save_while);
    read_flag_of(jv, key, flag);
    settings.save_while = flag;
}

void read_while_iterations_limit(boost::json::value const & jv, SnippySettings& settings)
{
    auto&& key = "while-iterations-limit";
    read_limit_of(jv, key, settings.while_iterations_limit);
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
    read_save_rem(jv, settings);
    read_save_while(jv, settings);
    read_while_iterations_limit(jv, settings);
    read_statements_limit(jv, settings);

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
