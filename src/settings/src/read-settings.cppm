module;

#include <filesystem>

export module test_generator_settings_reader;

export import test_generator_settings;

namespace test_generator
{

export
SnippySettings read_settings(std::filesystem::path const & settings)
{
    // TODO: 
    return SnippySettings{};
}

} /* namespace test_generator */



