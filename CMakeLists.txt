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

option(PARACL_SNIPPY_ENABLE_SANITIZERS "Enable Address/Undefined sanitizers" OFF)

if (PARACL_SNIPPY_ENABLE_SANITIZERS)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
      -g
      -O0
      -fno-omit-frame-pointer
      -fsanitize=address,undefined
    )

    add_link_options(
      -fsanitize=address,undefined
    )
  else()
    message(WARNING "Sanitizers are supported only for Clang/GCC in this setup")
  endif()
endif()

# =================================================================================================

set(TEST_GENERATOR_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# =================================================================================================

add_subdirectory(
    ${TEST_GENERATOR_SRC_DIR}/AST
    ${CMAKE_BINARY_DIR}/subproject/ast
)

add_subdirectory(${TEST_GENERATOR_SRC_DIR}/options)
add_subdirectory(${TEST_GENERATOR_SRC_DIR}/settings)
add_subdirectory(${TEST_GENERATOR_SRC_DIR}/builder)
add_subdirectory(${TEST_GENERATOR_SRC_DIR}/serializer)

# =================================================================================================

set(TEST_GENERATOR_EXE paracl-snippy)

add_executable(${TEST_GENERATOR_EXE})

target_sources(${TEST_GENERATOR_EXE}
PRIVATE
    ${TEST_GENERATOR_SRC_DIR}/main.cpp
)

target_link_libraries(${TEST_GENERATOR_EXE}
PRIVATE
    test-generator-options
    test-generator-options-reader
    test-generator-settings
    test-generator-settings-reader
    ast-random-builder
    ast-serializer
)

# =================================================================================================
