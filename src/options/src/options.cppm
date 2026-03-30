module;

#include <filesystem>
#include <stdexcept>

export module options;


namespace test_generator
{

export
struct Options
{
    std::filesystem::path output_file   = "a.cl";
    std::filesystem::path settings_file = "";

    void check_configuration() const
    {
        if (output_file.extension() != ".cl")
            throw std::runtime_error("Bad configuration: unsupported file format: output file must have ParaCL extension (.cl): '" + output_file.string() + "'");
        if (not settings_file.string().empty() and settings_file.extension() != ".json")
            throw std::runtime_error("Bad configuration: unsupported file format: settings file must have json extension (.json): '" + settings_file.string() + "'");
    }
};

} /* namespace test_generator */
