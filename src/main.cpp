#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <cstdlib>

import options;
import options_reader;
import settings;
import settings_reader;
import ast_builder;
import ast_serializer;

int main(int argc, char* argv[]) try
{
    auto&& options = test_generator::read_options(argc, argv);
    auto&& settings = test_generator::read_settings(options.settings_file);
    auto&& random_ast = test_generator::generate_random_ast(settings);
    test_generator::serialize(random_ast, options);
    return EXIT_SUCCESS;
}
catch (std::exception const & e)
{
    std::cerr << "ParaCL-snippy error: " << e.what() << "\n";
    return EXIT_FAILURE;
}
catch (...)
{
    std::cerr << "ParaCL-snippy: undefined error.\n";
    return EXIT_FAILURE;
}