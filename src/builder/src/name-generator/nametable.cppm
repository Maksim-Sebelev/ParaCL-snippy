module;

//---------------------------------------------------------------------------------------------------------------

#include <limits>
#include <cinttypes>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstddef>
#include <unordered_set>
#include <type_traits>
#include <iostream>
#include <cassert>

#define LOGINFO(...)
#define LOGERR(...)

//---------------------------------------------------------------------------------------------------------------

export module name_generator_nametable;

//---------------------------------------------------------------------------------------------------------------
namespace test_generator
{
//---------------------------------------------------------------------------------------------------------------

export
using unique_name_id_t = unsigned int;

//---------------------------------------------------------------------------------------------------------------

export
class Nametable
{
  private:
    using scopes_t = std::vector<std::unordered_set<unique_name_id_t>>;
    scopes_t scopes_;
    /* here size_t is not a unique_name_id_t */
    size_t unique_name_id_ = 0;
    size_t unique_names_at_all_time_quant_ = 0;

  public:
    void             new_scope             ();
    void             leave_scope           ();
    void             declare               (unique_name_id_t id);

    unique_name_id_t get_new_unique_name_id()                    const noexcept(std::is_nothrow_convertible_v<size_t, unique_name_id_t>);
    unique_name_id_t get_absolute_new_unique_name_id()           const noexcept(std::is_nothrow_convertible_v<size_t, unique_name_id_t>);

    size_t           size                  ()                    const noexcept(std::is_nothrow_convertible_v<size_t, unique_name_id_t>);
    bool             exists                (unique_name_id_t id) const;
    bool             empty                 ()                    const;

    std::vector<unique_name_id_t> get_existing_variables() const;

    friend void dump(Nametable const & nt)
    {
        std::cout << "Nametable dump\n{\n";
        std::cout << "unique names now: " << nt.unique_name_id_ << "\n";
        std::cout << "unique names at all time: " << nt.unique_names_at_all_time_quant_ << "\n";
        size_t it = 0;
        for (auto&& scope: nt.scopes_)
        {
            std::cout << "\tscope[" << it++ << "]\n";
            for (auto&& name: scope)
            {
                std::cout << "\t\t" << name << "\n";
            }
            std::cout << "\n";
        }
        std::cout << "}\n";
    }
};

//---------------------------------------------------------------------------------------------------------------

void Nametable::new_scope()
{
    LOGINFO("paracl: interpreter: nametable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::leave_scope()
{
    LOGINFO("paracl: interpreter: nametable: exiting scope");

    if (scopes_.empty()) throw std::runtime_error("Trying leave scope in empty nametable");

    auto&& counter = 0LU;

    for (auto&& name: scopes_.back())
    {
        if (name >= unique_name_id_) continue;
        ++counter;
    }

    unique_name_id_ -= counter;
    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

unique_name_id_t Nametable::get_new_unique_name_id() const noexcept(std::is_nothrow_convertible_v<size_t, unique_name_id_t>)
{
    return unique_name_id_;
}

//---------------------------------------------------------------------------------------------------------------

unique_name_id_t Nametable::get_absolute_new_unique_name_id() const noexcept(std::is_nothrow_convertible_v<size_t, unique_name_id_t>)
{
    return unique_names_at_all_time_quant_;
}

//---------------------------------------------------------------------------------------------------------------

bool Nametable::exists(unique_name_id_t id) const
{
    for (auto&& scope: scopes_)
    {
        if (not scope.contains(id)) continue;
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::declare(unique_name_id_t id)
{
    LOGINFO("paracl: interpreter: nametable: declate {} = \"{}\"", id);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    if (exists(id))
        throw std::runtime_error("Redeclarion of '" + std::to_string(id) + "'");

    scopes_.back().insert(id);

    if ((unique_name_id_ == unique_names_at_all_time_quant_) or (id < unique_names_at_all_time_quant_))
        ++unique_name_id_;

    ++unique_names_at_all_time_quant_;
}

//---------------------------------------------------------------------------------------------------------------

bool Nametable::empty() const
{
    for (auto&& scope: scopes_)
    {
        if (scope.empty()) continue;
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------------------------------------------

std::vector<unique_name_id_t> Nametable::get_existing_variables() const
{
    auto&& existing_variables = std::vector<unique_name_id_t>{};
    existing_variables.reserve(unique_name_id_);

    for (auto&& scope: scopes_)
        existing_variables.insert(existing_variables.end(), scope.begin(), scope.end());

    return existing_variables;
}

//---------------------------------------------------------------------------------------------------------------

size_t Nametable::size() const noexcept(std::is_nothrow_convertible_v<size_t, unique_name_id_t>)
{
    auto&& size = 0LU;

    for (auto&& scope: scopes_)
        size += scope.size();

    return size;
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator */
//---------------------------------------------------------------------------------------------------------------