module;

#include <ranges>
#include <string>
#include <cassert>
#include <cstddef>
#include <random>
#include <limits>
#include <stdexcept>
#include <iostream>

#include "create-basic-node.hpp"

export module name_generator;

//---------------------------------------------------------------------------------------------------------------

export import thelast;
export import name_generator_nametable;
import ast_serializer;

//---------------------------------------------------------------------------------------------------------------

SPECIALIZE_CREATE(last::node::Variable, last::node::dumpable, last::node::serializable)

//---------------------------------------------------------------------------------------------------------------
namespace test_generator
{
//---------------------------------------------------------------------------------------------------------------

export
class NameGenerator : private Nametable
{
public:
    enum : unique_name_id_t  { RandomExistingName = std::numeric_limits<unique_name_id_t>::max(), UniqueName = RandomExistingName - 1 };
private:
    static const constexpr std::string variables_name_base_ = "var_";

private:
    std::mt19937 &rng_;

private:
    using Nametable::get_existing_variables;

public:
    using Nametable::get_new_unique_name_id;
    using Nametable::get_absolute_new_unique_name_id;
    using Nametable::declare;
    using Nametable::new_scope;
    using Nametable::leave_scope;
    using Nametable::exists;
    using Nametable::empty;
    using Nametable::size;    

private:
    static std::string generate_name_by_id(unique_name_id_t id)
    { return variables_name_base_ + std::to_string(id); }

private:
    unique_name_id_t select_random_existing_variable_id() const
    {
        auto&& existing_variables = get_existing_variables();
        auto&& size = existing_variables.size();
        auto&& random_index = std::uniform_int_distribution<size_t>{0, size - 1}(rng_);
        return existing_variables[random_index];
    }

    last::node::BasicNode generate_variable(unique_name_id_t id)
    {
        assert(id != RandomExistingName);
        assert(id <= get_absolute_new_unique_name_id());

        if (not exists(id))
            declare(id);

        auto&& name = generate_name_by_id(id);

        return last::node::create(last::node::Variable{std::move(name)});
    }

public:
    NameGenerator(std::mt19937& randomizer) : rng_(randomizer)
    {}

public:
    last::node::BasicNode generate_new_variable()
    {
        return generate_variable(get_new_unique_name_id());
    }

    last::node::BasicNode generate_absolute_new_variable()
    {
        return generate_variable(get_absolute_new_unique_name_id());
    }

    last::node::BasicNode generate_existing_variable(unique_name_id_t id = RandomExistingName)
    {
        if (empty()) throw std::runtime_error("Requests generataion of existing varible with id = "+std::to_string(get_new_unique_name_id())+" in empty nametable");
        if (id == RandomExistingName)
        {
            id = select_random_existing_variable_id();
        }
        else if (id >= get_new_unique_name_id())
        {
            throw std::runtime_error("Requests generation of existing variable with id("+std::to_string(get_new_unique_name_id())+"), "
                                     "which is not respond to some existing variable.\n"
                                     "Now exists only "+std::to_string(get_new_unique_name_id())+" different variables");
        }

        return generate_variable(id);
    }

public:
    friend void dump(NameGenerator const & ng)
    {
        std::cout << "NameGenerator dump\n{\n";
        dump(static_cast<Nametable>(ng));
        std::cout << "}\n";
    }
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator */
//---------------------------------------------------------------------------------------------------------------
