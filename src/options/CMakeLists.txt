cmake_minimum_required(VERSION 3.28)


cmake_minimum_required(VERSION 3.28)

# =================================================================================================

project(ParaCL-snippy-options
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

find_package(Boost REQUIRED COMPONENTS program_options)

# =================================================================================================

set(PARACL_SNIPPY_OPTIONS_LIB test-generator-options)
add_library(${PARACL_SNIPPY_OPTIONS_LIB})

set(PARACL_SNIPPY_OPTIONS_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PARACL_SNIPPY_OPTIONS_SRCS
    ${PARACL_SNIPPY_OPTIONS_SRC_DIR}/options.cppm
)

target_sources(${PARACL_SNIPPY_OPTIONS_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${PARACL_SNIPPY_OPTIONS_SRCS}
)

# =================================================================================================

set(PARACL_SNIPPY_OPTIONS_READER_LIB test-generator-options-reader)
add_library(${PARACL_SNIPPY_OPTIONS_READER_LIB})

set(PARACL_SNIPPY_OPTIONS_READER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PARACL_SNIPPY_OPTIONS_READER_SRCS
    ${PARACL_SNIPPY_OPTIONS_READER_SRC_DIR}/read-options.cppm
)

target_sources(${PARACL_SNIPPY_OPTIONS_READER_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${PARACL_SNIPPY_OPTIONS_READER_SRCS}
)

target_link_libraries(${PARACL_SNIPPY_OPTIONS_READER_LIB}
PUBLIC
    ${PARACL_SNIPPY_OPTIONS_LIB}
PRIVATE
    Boost::program_options
)

target_compile_definitions(${PARACL_SNIPPY_OPTIONS_READER_LIB}
PRIVATE
    -DPARACL_SNIPPY_VERSION="${PARACL_SNIPPY_VERSION}"
)

# =================================================================================================