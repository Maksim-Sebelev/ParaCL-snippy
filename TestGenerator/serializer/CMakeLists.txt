cmake_minimum_required(VERSION 3.30)

# =================================================================================================

project(ParaCL-snippy-ast-serializer
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

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/AST
    ${CMAKE_BINARY_DIR}/subproject/ast
)

# =================================================================================================

set(PARACL_SNIPPY_AST_SERIALIZER_LIB ast-serializer)
add_library(${PARACL_SNIPPY_AST_SERIALIZER_LIB})

set(PARACL_SNIPPY_AST_SERIALIZER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PARACL_SNIPPY_AST_SERIALIZER_SRCS
    ${PARACL_SNIPPY_AST_SERIALIZER_SRC_DIR}/serializer.cppm
)

target_sources(${PARACL_SNIPPY_AST_SERIALIZER_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${PARACL_SNIPPY_AST_SERIALIZER_SRCS}
)

target_link_libraries(${PARACL_SNIPPY_AST_SERIALIZER_LIB}
PUBLIC
    TheLast::TheLast
)

# =================================================================================================
