cmake_minimum_required(VERSION 3.30)

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

set(PARACL_SNIPPY_SETTINGS_LIB test-generator-settings)
add_library(${PARACL_SNIPPY_SETTINGS_LIB})

set(PARACL_SNIPPY_SETTINGS_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PARACL_SNIPPY_SETTINGS_SRCS
    ${PARACL_SNIPPY_SETTINGS_SRC_DIR}/settings.cppm
)

target_sources(${PARACL_SNIPPY_SETTINGS_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${PARACL_SNIPPY_SETTINGS_SRCS}
)

# =================================================================================================

set(PARACL_SNIPPY_SETTINGS_READER_LIB test-generator-settings-reader)
add_library(${PARACL_SNIPPY_SETTINGS_READER_LIB})

set(PARACL_SNIPPY_SETTINGS_READER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PARACL_SNIPPY_SETTINGS_READER_SRCS
    ${PARACL_SNIPPY_SETTINGS_READER_SRC_DIR}/read-settings.cppm
)

target_sources(${PARACL_SNIPPY_SETTINGS_READER_LIB}
PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${PARACL_SNIPPY_SETTINGS_READER_SRCS}
)

target_link_libraries(${PARACL_SNIPPY_SETTINGS_READER_LIB}
PUBLIC
    ${PARACL_SNIPPY_SETTINGS_LIB}
)


# =================================================================================================
