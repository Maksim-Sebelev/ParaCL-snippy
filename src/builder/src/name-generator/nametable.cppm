module;

//---------------------------------------------------------------------------------------------------------------

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
    bool unique_names_at_all_time_quant_is_greater_than_unique_name_id = false;

  public:
    void             new_scope             ();
    void             leave_scope           ();
    void             declare               (unique_name_id_t id);

    unique_name_id_t get_new_unique_name_id()                    const noexcept(std::is_nothrow_copy_constructible_v<unique_name_id_t>);
    unique_name_id_t get_absolute_new_unique_name_id()           const noexcept(std::is_nothrow_copy_constructible_v<unique_name_id_t>);

    bool             exists                (unique_name_id_t id) const;
    bool             empty                 ()                    const;

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

    auto&& back = scopes_.back();
    if (not back.empty())
        unique_names_at_all_time_quant_is_greater_than_unique_name_id = true;

    // auto&& counter = 0LU;

    // for (auto&& name: back)
    // {
    //     if (name >= unique_name_id_) continue;
    //     ++counter;
    // }

    unique_name_id_ -= back.size();
    // unique_name_id_ -= counter;
    scopes_.pop_back(); // в одном scope создали id = 0, вышел из scope
                        // считчик уменьшился и в новом scope сново содается id = 0
    
}

//---------------------------------------------------------------------------------------------------------------

unique_name_id_t Nametable::get_new_unique_name_id() const noexcept(std::is_nothrow_copy_constructible_v<unique_name_id_t>)
{
    return unique_name_id_;
}

//---------------------------------------------------------------------------------------------------------------

unique_name_id_t Nametable::get_absolute_new_unique_name_id() const noexcept(std::is_nothrow_copy_constructible_v<unique_name_id_t>)
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
    else { assert(0); }
    if (unique_names_at_all_time_quant_ < unique_name_id_)
        unique_names_at_all_time_quant_ = unique_name_id_;
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
} /* namespace test_generator */
//---------------------------------------------------------------------------------------------------------------
