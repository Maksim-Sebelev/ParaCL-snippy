module;

#include <ranges>
#include <string>
#include <cassert>
#include <cstddef>
#include <random>
#include <limits>
#include <stdexcept>

#include "create-basic-node.hpp"

export module name_generator;

//---------------------------------------------------------------------------------------------------------------

export import thelast;
import name_generator_nametable;
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
    enum : unique_name_id_t  { RandomExistingName = std::numeric_limits<unique_name_id_t>::max() };
private:
    static const constexpr std::string variables_name_base_ = "var_";

private:
    std::mt19937 &rng_;

private:
public:
    using Nametable::get_new_unique_name_id;
    using Nametable::declare;
    using Nametable::new_scope;
    using Nametable::leave_scope;
    using Nametable::exists;
    using Nametable::empty;

private:
    static std::string generate_name_by_id(unique_name_id_t id)
    { return variables_name_base_ + std::to_string(id); }

private:
    unique_name_id_t select_random_existing_variable_id() const
    { return std::uniform_int_distribution<unique_name_id_t>{static_cast<unique_name_id_t>(0), get_new_unique_name_id()}(rng_); }

    last::node::BasicNode generate_variable(unique_name_id_t id)
    {
        assert(id != RandomExistingName);
        assert(id <= get_new_unique_name_id());

        if (id == get_new_unique_name_id())
            declare(id);

        auto&& name = generate_name_by_id(id);

        return last::node::create(last::node::Variable{std::move(name)});
    }

public:
    NameGenerator(std::mt19937& randomizer) : rng_(randomizer)
    { new_scope(); /* global scope */ }

public:
    last::node::BasicNode generate_new_variable()
    {
        return generate_variable(get_new_unique_name_id());
    }

    last::node::BasicNode generate_existing_variable(unique_name_id_t id = RandomExistingName)
    {
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
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator */
//---------------------------------------------------------------------------------------------------------------
