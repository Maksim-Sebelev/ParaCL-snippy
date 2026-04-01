cmake_minimum_required(VERSION 3.28)

# =================================================================================================

project(ParaCL-snippy-tests-generator
    VERSION 1.0
)

# =================================================================================================

if (NOT PARACL_SNIPPY)
    message(FATAL_ERROR "'PARACL_SNIPPY' not set. Failed find paracl-snippy")
endif (NOT PARACL_SNIPPY)


# =================================================================================================

set(PARACL_SNIPPY_BASIC_SETTINGS          ${CMAKE_SOURCE_DIR}/basic-settings)

set(PARACL_SNIPPY_TESTS_GENERATOR_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PARACL_SNIPPY_TESTS_GENERATOR_SRC_IN  ${PARACL_SNIPPY_TESTS_GENERATOR_SRC_DIR}/generate-basic-tests.sh.in)
set(PARACL_SNIPPY_TESTS_GENERATOR_SRC_OUT ${CMAKE_BINARY_DIR}/generate-basic-tests)

# =================================================================================================

configure_file(
    ${PARACL_SNIPPY_TESTS_GENERATOR_SRC_IN}
    ${PARACL_SNIPPY_TESTS_GENERATOR_SRC_OUT}
    @ONLY
)

file(CHMOD
    ${PARACL_SNIPPY_TESTS_GENERATOR_SRC_OUT}
    PERMISSIONS
        OWNER_READ
        OWNER_EXECUTE
)

# =================================================================================================
