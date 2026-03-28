#include <iostream>
#include <stdexcept>
#include <cstdlib>

import test_generator_settings;
import ast_builder;
import ast_serializer;

int main() try
{
    test_generator::SnippySettings settings{};
    auto random_ast = test_generator::generate_random_ast(settings);
    test_generator::serialize(random_ast, settings);
    return EXIT_SUCCESS;
}
catch (std::exception const& e)
{
    std::cerr << "ParaCL-snippy error: " << e.what() << "\n";
    return EXIT_FAILURE;
}
catch (...)
{
    std::cerr << "ParaCL-snippy: undefined error.\n";
    return EXIT_FAILURE;
}