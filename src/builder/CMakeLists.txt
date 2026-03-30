cmake_minimum_required(VERSION 3.28)

# =================================================================================================

project(ParaCL-snippy-name-generator
    LANGUAGES CXX
    VERSION 1.0
)

# =================================================================================================

if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
else()
  if (CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "CMAKE_CXX_STANDARD requires to be greater or equal than 20, but given ${CMAKE_CXX_STANDARD}")
  endif(CMAKE_CXX_STANDARD LESS 20)
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# =================================================================================================

set(PARACL_SNIPPY_NAME_GENERATOR_LIB name-generator)
add_library(${PARACL_SNIPPY_NAME_GENERATOR_LIB})

set(PARACL_SNIPPY_NAME_GENERATOR_SRC_DIR ${PROJECT_SOURCE_DIR}/src/name-generator)
set(PARACL_SNIPPY_NAME_GENERATOR_SRCS
    ${PARACL_SNIPPY_NAME_GENERATOR_SRC_DIR}/nametable.cppm
    ${PARACL_SNIPPY_NAME_GENERATOR_SRC_DIR}/name-generator.cppm
)

target_sources(${PARACL_SNIPPY_NAME_GENERATOR_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE     CXX_MODULES
    FILES
        ${PARACL_SNIPPY_NAME_GENERATOR_SRCS}
)

target_link_libraries(${PARACL_SNIPPY_NAME_GENERATOR_LIB}
PUBLIC
    TheLast::TheLast
PRIVATE
    ast-serializer
)

# =================================================================================================

set(PARACL_SNIPPY_AST_BUILDER_LIB ast-random-builder)
add_library(${PARACL_SNIPPY_AST_BUILDER_LIB})

set(PARACL_SNIPPY_AST_BUILDER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PARACL_SNIPPY_AST_BUILDER_SRCS
    ${PARACL_SNIPPY_AST_BUILDER_SRC_DIR}/builder.cppm
)

target_sources(${PARACL_SNIPPY_AST_BUILDER_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${PARACL_SNIPPY_AST_BUILDER_SRCS}
)

target_link_libraries(${PARACL_SNIPPY_AST_BUILDER_LIB}
PUBLIC
    TheLast::TheLast
PRIVATE
    ${PARACL_SNIPPY_NAME_GENERATOR_LIB}
    ${PARACL_SNIPPY_SETTINGS_READER_LIB}
    test-generator-settings
    ast-serializer
)

# =================================================================================================

