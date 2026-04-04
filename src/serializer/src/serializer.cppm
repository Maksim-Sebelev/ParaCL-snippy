module;

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <random>

export module ast_serializer;

export import thelast;
export import options;

namespace test_generator
{

/* 'export' for last::node::serializable */
export
enum class BuildProgramSetting
{
    DummyValue = 0,
    NoEnclosure,
    UseEnclosure,
    NotFirstIf,
    ScopeLikeConditionBody,
    GlobalScope,
    DontSeparateExpression,
};

void write_n_tab(std::ostream& os, size_t tabs)
{ os << "\n" << std::string(tabs * 4, ' '); }

void new_line(std::ostream& os)
{ os << "\n"; }

void begin_scope(std::ostream& os, size_t enclosure)
{
    write_n_tab(os, enclosure);
    os << "{";
}

void leave_scope(std::ostream& os, size_t enclosure)
{
    write_n_tab(os, enclosure);
    os << "}";
}

bool is_not_expression(BuildProgramSetting setting)
{
    return ((setting != BuildProgramSetting::NoEnclosure) and (setting != BuildProgramSetting::DontSeparateExpression));
}

void expression_or_statement_begin_action(std::ostream& os, size_t enclosure, BuildProgramSetting setting)
{
    auto&& is_expression            = (setting == BuildProgramSetting::NoEnclosure);
    auto&& dont_separate_expression = (setting == BuildProgramSetting::DontSeparateExpression);

    if (dont_separate_expression) return;

    if (is_expression)
        os << "(";
    else
        write_n_tab(os, enclosure);
}

void expression_or_statement_end_action(std::ostream& os, BuildProgramSetting setting)
{
    auto&& is_expression            = (setting == BuildProgramSetting::NoEnclosure);
    auto&& dont_separate_expression = (setting == BuildProgramSetting::DontSeparateExpression);

    if (dont_separate_expression) return;

    if (is_expression)
        os << ")";
    else
        os << ";";
}


void statement_end(std::ostream& os)
{ os << ";"; }


} /* namespace serialize */

namespace last::node
{

export using serializable = void(std::ostream&, size_t, test_generator::BuildProgramSetting);

auto serialize(BasicNode const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
    -> decltype(visit<void, std::ostream&, size_t, test_generator::BuildProgramSetting>(node, os, enclosure, setting))
{
    return visit<void, std::ostream&, size_t, test_generator::BuildProgramSetting>(node, os, enclosure, setting);
}

namespace visit_specializations
{

template <>
void visit(Print const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    expression_or_statement_begin_action(os, enclosure, setting);

    os << "print ";

    if (node.size() != 0)
    {
        serialize(node[0], os, 0, test_generator::BuildProgramSetting::DontSeparateExpression);
        for (auto&& it = 1LU, ite = node.size(); it < ite; ++it)
        {
            os << ", ";
            serialize(node[it], os, enclosure, test_generator::BuildProgramSetting::DontSeparateExpression);
        }
    }

    expression_or_statement_end_action(os, setting);
}

template <>
void visit([[maybe_unused]] Scan const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    auto&& is_not_expression = test_generator::is_not_expression(setting);
    if (is_not_expression)
        test_generator::write_n_tab(os, enclosure);

    os << "?";

    if (is_not_expression)
        test_generator::statement_end(os);
}

template <>
void visit(Scope const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    auto&& size = node.size();

    auto&& is_scope_after_condition = (setting == test_generator::BuildProgramSetting::ScopeLikeConditionBody);
    auto&& is_scope_global          = (setting == test_generator::BuildProgramSetting::GlobalScope);

    if ((size == 0) and is_scope_after_condition)
    {
        test_generator::write_n_tab(os, enclosure);
        os << "{}";
        return;
    }

    auto&& need_brackets = not (is_scope_global);
    size_t offset = static_cast<size_t>(not is_scope_global);

    if (need_brackets)
        test_generator::begin_scope(os, enclosure);

    for (auto&& arg: node)
        serialize(arg, os, enclosure + offset, test_generator::BuildProgramSetting::UseEnclosure);

    if (need_brackets)
        test_generator::leave_scope(os, enclosure);
}

template <>
void visit(LinearSequence const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    for (auto&& arg: node)
        serialize(arg, os, enclosure, test_generator::BuildProgramSetting::UseEnclosure);
}

template <>
void visit(While const & node, std::ostream& os, size_t enclosure, [[maybe_unused]] test_generator::BuildProgramSetting)
{
    test_generator::write_n_tab(os, enclosure);
    os << "while (";
    serialize(node.condition(), os, 0, test_generator::BuildProgramSetting::DontSeparateExpression);
    os << ")";
    serialize(node.body(), os, enclosure, test_generator::BuildProgramSetting::ScopeLikeConditionBody);
}

template <>
void visit(If const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    test_generator::write_n_tab(os, enclosure);
    if (setting == test_generator::BuildProgramSetting::NotFirstIf)
        os << "else ";

    os << "if (";
    serialize(node.condition(), os, 0, test_generator::BuildProgramSetting::DontSeparateExpression);
    os << ")";
    serialize(node.body(), os, enclosure, test_generator::BuildProgramSetting::ScopeLikeConditionBody);
}

template <>
void visit(Else const & node, std::ostream& os, size_t enclosure, [[maybe_unused]] test_generator::BuildProgramSetting)
{
    test_generator::write_n_tab(os, enclosure);
    os << "else";
    serialize(node.body(), os, enclosure, test_generator::BuildProgramSetting::ScopeLikeConditionBody);
}

template <>
void visit(Condition const & node, std::ostream& os, size_t enclosure, [[maybe_unused]] test_generator::BuildProgramSetting setting)
{
    if (node.get_ifs().size() != 0)
    {
        auto&& ifs = node.get_ifs();
        serialize(ifs[0], os, enclosure, test_generator::BuildProgramSetting::DummyValue);
        for (auto&& it = 1LU, ite = ifs.size(); it < ite; ++it)
            serialize(ifs[it], os, enclosure, test_generator::BuildProgramSetting::NotFirstIf);
    }

    if (node.has_else())
        serialize(node.get_else(), os, enclosure, test_generator::BuildProgramSetting::DummyValue);
}

template <>
void visit(BinaryOperator const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    expression_or_statement_begin_action(os, enclosure, setting);

    serialize(node.larg(), os, enclosure, test_generator::BuildProgramSetting::NoEnclosure);
    switch (node.type())
    {
        case BinaryOperator::AND    : os << " && "; break;
        case BinaryOperator::OR     : os << " || "; break;
        case BinaryOperator::ADD    : os << " + " ; break;
        case BinaryOperator::SUB    : os << " - " ; break;
        case BinaryOperator::MUL    : os << " * " ; break;
        case BinaryOperator::DIV    : os << " / " ; break;
        case BinaryOperator::REM    : os << " % " ; break;
        case BinaryOperator::ISAB   : os << " > " ; break;
        case BinaryOperator::ISABE  : os << " >= "; break;
        case BinaryOperator::ISLS   : os << " < " ; break;
        case BinaryOperator::ISLSE  : os << " <= "; break;
        case BinaryOperator::ISEQ   : os << " == "; break;
        case BinaryOperator::ISNE   : os << " != "; break;
        case BinaryOperator::ASGN   : os << " = " ; break;
        case BinaryOperator::ADDASGN: os << " += "; break;
        case BinaryOperator::SUBASGN: os << " -= "; break;
        case BinaryOperator::MULASGN: os << " *= "; break;
        case BinaryOperator::DIVASGN: os << " /= "; break;
        case BinaryOperator::REMASGN: os << " %= "; break;
        default: __builtin_unreachable();
    }
    serialize(node.rarg(), os, enclosure, test_generator::BuildProgramSetting::NoEnclosure);

    expression_or_statement_end_action(os, setting);
}

template <>
void visit(UnaryOperator const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    auto&& is_not_expression = test_generator::is_not_expression(setting);
    if (is_not_expression)
        test_generator::write_n_tab(os, enclosure);

    switch (node.type())
    {
        case UnaryOperator::PLUS : os << "+"; break;
        case UnaryOperator::MINUS: os << "-"; break;
        case UnaryOperator::NOT  : os << "!"; break;
        default: __builtin_unreachable();
    }

    serialize(node.arg(), os, enclosure, test_generator::BuildProgramSetting::NoEnclosure);

    if (is_not_expression)
        test_generator::statement_end(os);
}

template <>
void visit(Variable const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    auto&& is_not_expression = test_generator::is_not_expression(setting);
    if (is_not_expression)
        test_generator::write_n_tab(os, enclosure);

    os << node.name();

    if (is_not_expression)
        test_generator::statement_end(os);
}

template <>
void visit(NumberLiteral const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    auto&& is_not_expression = test_generator::is_not_expression(setting);
    if (is_not_expression)
        test_generator::write_n_tab(os, enclosure);

    os << node.value();

    if (is_not_expression)
        test_generator::statement_end(os);
}

template <>
void visit(StringLiteral const & node, std::ostream& os, [[maybe_unused]] size_t enclosure, [[maybe_unused]] test_generator::BuildProgramSetting)
{
    os << "\"" << node.value() << "\"";
}

template <>
void visit(Return const & node, std::ostream& os, size_t enclosure, [[maybe_unused]] test_generator::BuildProgramSetting)
{
    test_generator::write_n_tab(os, enclosure);
    os << "return ";
    serialize(node.expression(), os, 0, test_generator::BuildProgramSetting::DontSeparateExpression);
    test_generator::statement_end(os);
}

template <>
void visit(FunctionDeclaration const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    auto&& is_not_expression = test_generator::is_not_expression(setting);
    if (is_not_expression)
        test_generator::write_n_tab(os, enclosure);

    os << node.name() << "func(";
    auto&& args = node.args();
    if (args.size() != 0)
    {
        os << args[0];
        for (auto&& it = 1LU, ite = args.size(); it != ite; ++it)
        {
            os << ", ";
            os << args[it];
        }
    }
    os << ")";

    if (not node.name().empty())
        os << " : " << node.name();

    serialize(node.body(), os, enclosure, test_generator::BuildProgramSetting::DummyValue);
}

template <>
void visit(FunctionCall const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    auto&& is_not_expression = test_generator::is_not_expression(setting);
    if (is_not_expression)
        test_generator::write_n_tab(os, enclosure);

    os << node.name() << "(";
    auto&& args = node.args();
    if (args.size() != 0)
    {
        serialize(args[0], os, 0, test_generator::BuildProgramSetting::DontSeparateExpression);
        for (auto&& it = 1LU, ite = args.size(); it != ite; ++it)
        {
            os << ", ";
            serialize(args[it], os, 0, test_generator::BuildProgramSetting::DontSeparateExpression);
        }
    }
    os << ")";

    if (is_not_expression)
        test_generator::statement_end(os);
}

template <>
void visit(Comment const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting setting)
{
    auto&& is_multi_line = (node.type() == Comment::MultiLine);
    auto&& is_separated = node.separated();

    if (is_separated)
        test_generator::write_n_tab(os, enclosure);
    else
        os << " ";

    if (is_multi_line)
        os << "/*";
    else
        os << "//";

    os << node.comment();

    if (is_multi_line)
    {
        if (is_separated)
            test_generator::write_n_tab(os, enclosure);
        os << "*/";
    }
}

template <>
void visit(Semicolon const & node, std::ostream& os, size_t enclosure, test_generator::BuildProgramSetting)
{
    auto&& is_separated = node.separated();

    if (is_separated)
        test_generator::write_n_tab(os, enclosure);

    os << ";";
}

} /* namespace visit_specializations */
} /* namespace last::node */

namespace test_generator
{

export
void serialize(last::AST const & ast, test_generator::Options const & options)
{
    auto&& ofs = std::ofstream{options.output_file};
    if (ofs.fail())
        throw std::runtime_error("Failed open: " + options.output_file.string() + " for ast serializing");

    auto&& now = std::chrono::system_clock::now();

    auto&& t_c      = std::chrono::system_clock::to_time_t(now);
    auto&& tm_local = *std::localtime(&t_c);

    ofs << "// Automatic generated with '" << "ParaCL-snippy" << "'\n"
        << "// " << std::put_time(&tm_local, "%Y-%m-%d %H:%M:%S") << "\n";

    if (not options.settings_file.string().empty())
    {
        auto&& ifs = std::ifstream{options.settings_file};
        if (ifs.fail()) throw std::runtime_error("No such file: '" + options.settings_file.string() + "'");

        ofs << "\n// generating settings: " << options.settings_file << ":\n/*\n";
        ofs << ifs.rdbuf();
        ofs << "*/\n\n";
    }
    else
    {
        ofs << "// generate with default settings.\n\n";
    }

    last::node::serialize(ast.root(), ofs, 0, test_generator::BuildProgramSetting::GlobalScope);
    ofs << "\n";
}

} /* namespace test_generator */
