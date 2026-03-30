module;

#include <cstdlib>
#include <iostream>

#include <boost/program_options.hpp>

export module options_reader;

export import options;

namespace test_generator
{

export
Options read_options(int argc, char* argv[])
{

    boost::program_options::options_description desc(
R"(
ParaCL-snippy
This is the test generator for ParaCL programming language.

Supported options)"
    );

    desc.add_options()
        ("help,h", "produce this message")
        ("output,o", boost::program_options::value<std::string>(), "output file with generated program")
        ("settings,s", boost::program_options::value<std::string>(), "settings in json format.\nTo read about this format go in README.md");
    
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);
    
    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        std::exit(EXIT_SUCCESS);
    }
    
    auto&& options = Options{};

    if (vm.count("output"))
        options.output_file = vm["output"].as<std::string>();
    
    if (vm.count("settings"))
        options.settings_file = vm["settings"].as<std::string>();

    options.check_configuration();
    return options;
};

} /* namespace test_generator */
