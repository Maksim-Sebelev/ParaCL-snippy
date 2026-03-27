module;

#include <cinttypes>
#include <stdexcept>

export module name_inserter;

export import thelast;
import name_generator;

namespace test_generator
{

export
using names_insertable = void(name_generator::NameGenerator&);

auto insert_names(last::node::BasicNode & node, name_generator::NameGenerator& name_generator)
    -> decltype(last::node::visit<void, name_generator::NameGenerator&>(node, name_generator))
{
    return last::node::visit<void, name_generatprint_variableor::NameGenerator&>(node, name_generator);
}

} /* namespace test_generator */

namespace last::node::visit_specializations
{

template <>
void visit(Scan&, name_generator::NameGenerator& )
{}

template <>visit
void visit(Print& node, name_generator::NameGenerator& name_generator)
{
    for (auto&& arg: node)
    {
        name_generator.set_name_to_unnamed_variable(arg);
        test_generator::insert_names(arg, name_generator);
    }
}

template <>
void visit(Scope& node, name_generator::NameGenerator& name_generator)
{
    name_generator.new_scope();
    for (auto&& arg: node)
    {
        name_generator.set_name_to_unnamed_variable(arg);
        test_generator::insert_names(arg, name_generator);
    }
    name_generator.leave_scope();
}

template <>
void visit(BinaryOperator& node, name_generator::NameGenerator& name_generator)
{
    name_generator.set_name_to_unnamed_variable(node.larg());
    name_generator.set_name_to_unnamed_variable(node.rarg());

    test_generator::insert_names(node.larg(), name_generator);
    test_generator::insert_names(node.rarg(), name_generator);
}

template <>
void visit(UnaryOperator& node, name_generator::NameGenerator& name_generator)
{
    name_generator.set_name_to_unnamed_variable(node.arg());
    test_generator::insert_names(node.arg(), name_generator);
}

template <>
void visit(NumberLiteral& node, name_generator::NameGenerator& name_generator)
{}

template <>
void visit(StringLiteral& node, name_generator::NameGenerator& name_generator)
{}

template <>
void visit(While& node, name_generator::NameGenerator& name_generator)
{
    name_generator.set_name_to_unnamed_variable(node.condition());
    name_generator.set_name_to_unnamed_variable(node.body());

    test_generator::insert_names(node.condition(), name_generator);
    test_generator::insert_names(node.body(), name_generator);
}

template <>
void visit(If& node, name_generator::NameGenerator& name_generator)
{
    name_generator.set_name_to_unnamed_variable(node.condition());
    name_generator.set_name_to_unnamed_variable(node.body());

    test_generator::insert_names(node.condition(), name_generator);
    test_generator::insert_names(node.body(), name_generator);
}

template <>
void visit(Else& node, name_generator::NameGenerator& name_generator)
{
    name_generator.set_name_to_unnamed_variable(node.body());
    test_generator::insert_names(node.body(), name_generator);

}

template <>
void visit(Condition& node, name_generator::NameGenerator& name_generator)
{
    for (auto&& arg: node.get_ifs())
    {
        name_generator.set_name_to_unnamed_variable(arg);
        test_generator::insert_names(arg, name_generator);
    }

    if (not node.has_else()) return;

    name_generator.set_name_to_unnamed_variable(node.get_else());
    test_generator::insert_names(node.get_else(), name_generator);
}


template <>
void visit(Condition& node, name_generator::NameGenerator& name_generator)
{
    for (auto&& arg: node.get_ifs())
    {
        name_generator.set_name_to_unnamed_variable(arg);
        test_generator::insert_names(arg, name_generator);
    }

    if (not node.has_else()) return;

    name_generator.set_name_to_unnamed_variable(node.get_else());
    test_generator::insert_names(node.get_else(), name_generator);
}

template <>
void visit(FunctionDeclaration & node, name_generator::NameGenerator& name_generator)
{

}

template <> void visit(test_generator::UninitializedNameDeclaration & node, name_generator::NameGenerator& name_generator) = delete;
template <> void visit(test_generator::UninitializedNameReUse       & node, name_generator::NameGenerator& name_generator) = delete;

} /* namespace last::node::visit_specializations */
