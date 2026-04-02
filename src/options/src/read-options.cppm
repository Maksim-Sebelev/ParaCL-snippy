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
        ("version,v", "show version of project")
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
    
    if (vm.count("version"))
    {
        std::cout <<
R"~(
 [0;1;31;91mm[0;1;33;93mmm[0;1;32;92mmm[0m                         [0;1;36;96mm[0;1;34;94mmm[0m  [0;1;31;91mm[0m                             [0;1;36;96m"[0m                        
 [0;1;33;93m#[0m   [0;1;36;96m"[0;1;34;94m#[0m  [0;1;35;95mm[0;1;31;91mmm[0m    [0;1;36;96mm[0m [0;1;34;94mmm[0m   [0;1;31;91mm[0;1;33;93mmm[0m   [0;1;36;96mm[0;1;34;94m"[0m   [0;1;31;91m"[0m [0;1;33;93m#[0m              [0;1;32;92mm[0;1;36;96mmm[0m   [0;1;35;95mm[0m [0;1;31;91mm[0;1;33;93mm[0m   [0;1;36;96mmm[0;1;34;94mm[0m    [0;1;31;91mm[0;1;33;93mmm[0;1;32;92mm[0m   [0;1;34;94mmm[0;1;35;95mmm[0m   [0;1;33;93mm[0m   [0;1;36;96mm[0m 
 [0;1;32;92m#[0;1;36;96mmm[0;1;34;94mm#[0;1;35;95m"[0m [0;1;31;91m"[0m   [0;1;32;92m#[0m   [0;1;34;94m#"[0m  [0;1;31;91m"[0m [0;1;33;93m"[0m   [0;1;36;96m#[0m  [0;1;34;94m#[0m      [0;1;32;92m#[0m             [0;1;36;96m#[0m   [0;1;35;95m"[0m  [0;1;31;91m#[0;1;33;93m"[0m  [0;1;32;92m#[0m    [0;1;35;95m#[0m    [0;1;33;93m#[0;1;32;92m"[0m [0;1;36;96m"#[0m  [0;1;35;95m#"[0m [0;1;31;91m"[0;1;33;93m#[0m  [0;1;32;92m"[0;1;36;96mm[0m [0;1;34;94mm"[0m 
 [0;1;36;96m#[0m      [0;1;33;93mm"[0;1;32;92m""[0;1;36;96m#[0m   [0;1;35;95m#[0m     [0;1;32;92mm"[0;1;36;96m""[0;1;34;94m#[0m  [0;1;35;95m#[0m      [0;1;36;96m#[0m       [0;1;33;93m""[0;1;32;92m"[0m    [0;1;34;94m"[0;1;35;95m""[0;1;31;91mm[0m  [0;1;33;93m#[0m   [0;1;36;96m#[0m    [0;1;31;91m#[0m    [0;1;32;92m#[0m   [0;1;34;94m#[0m  [0;1;31;91m#[0m   [0;1;32;92m#[0m   [0;1;34;94m#m[0;1;35;95m#[0m  
 [0;1;34;94m#[0m      [0;1;32;92m"m[0;1;36;96mm"[0;1;34;94m#[0m   [0;1;31;91m#[0m     [0;1;36;96m"m[0;1;34;94mm"[0;1;35;95m#[0m   [0;1;33;93m"m[0;1;32;92mmm[0;1;36;96m"[0m [0;1;34;94m#m[0;1;35;95mmm[0;1;31;91mmm[0m        [0;1;35;95m"m[0;1;31;91mmm[0;1;33;93m"[0m  [0;1;32;92m#[0m   [0;1;34;94m#[0m  [0;1;31;91mmm[0;1;33;93m#m[0;1;32;92mm[0m  [0;1;36;96m#[0;1;34;94m#m[0;1;35;95m#"[0m  [0;1;33;93m##[0;1;32;92mm#[0;1;36;96m"[0m   [0;1;35;95m"#[0m   
                                                                       [0;1;34;94m#[0m      [0;1;32;92m#[0m       [0;1;31;91mm"[0m   
                                                                       [0;1;35;95m"[0m      [0;1;36;96m"[0m      [0;1;31;91m"[0;1;33;93m"[0m    
)~" <<
#if defined(PARACL_SNIPPY_VERSION)
        "Version: " << PARACL_SNIPPY_VERSION <<
#endif /* defined (PARACL_SNIPPY_VERSION) */
R"~(
This is the open-source proejct by MIPT students for MIPT students.
You can use it free without any restrictions,
to test your ParaCL compiler or interpreter for MIPT bachelor C++ course
from Konstantin Vladimirov (https://github.com/tilir)
)~";
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
