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
#include <algorithm>
#include <iterator>
#include <numeric>

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
    using scope_t = std::unordered_set<unique_name_id_t>;
    using scopes_t = std::vector<scope_t>;
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
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::leave_scope()
{
    if (scopes_.empty()) throw std::runtime_error("Trying leave scope in empty nametable");

    auto&& last_scope = scopes_.back();
    auto&& unique_for_scope_names =
        std::count_if
        (
            last_scope.begin(),
            last_scope.end(),
            [this](unique_name_id_t id) { return id < unique_name_id_; }
        );

    unique_name_id_ -= unique_for_scope_names;

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
    return
        std::any_of
        (
            scopes_.begin(),
            scopes_.end(),
            [id](scope_t const & scope)
            { return scope.contains(id); }
        );
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::declare(unique_name_id_t id)
{
    if (scopes_.empty())
       throw std::runtime_error("cannot declare variable (" + std::to_string(id) + "): no active scopes");

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
    return
        std::all_of
        (
            scopes_.begin(),
            scopes_.end(),
            [](scope_t const & scope) { return scope.empty(); }
        );
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
    return std::transform_reduce(scopes_.begin(), scopes_.end(), 0LU, 
        std::plus<>(),
        [](const scope_t& scope) { return scope.size(); }
    );
}

//---------------------------------------------------------------------------------------------------------------
} /* namespace test_generator */
//---------------------------------------------------------------------------------------------------------------