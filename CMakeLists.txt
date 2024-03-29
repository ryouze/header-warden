# Minimum required version of CMake (3.19 is the first version to support Apple Silicon)
cmake_minimum_required(VERSION 3.19)

# Project name
project(
    header-warden
    VERSION 1.0
    LANGUAGES CXX
)

# Require out-of-source builds
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# C++ standard and requirement
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type: Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Set the possible values of build type for cmake-gui
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

# Ensure only Debug or Release build types are allowed
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
    message(FATAL_ERROR "Invalid build type. Allowed types are: Debug, Release.")
endif()

# Set optimization level to -O3 for Release build type
add_compile_options("$<$<CONFIG:RELEASE>:-O3>")

# Add compile options for all targets
# -Wall: Enable all warnings
# -Wextra: Enable extra warnings that are not enabled by -Wall
# -Werror: Treat warnings as errors
# -Wpedantic: Request strict ISO C++ compliance
add_compile_options(-Wall -Wextra -Werror -Wpedantic)

# Enable all warnings for all targets (Clang only)
# -Weverything: Enable all warnings
# -Wno-c++98-compat: Disable C++98 compatibility warnings (target is C++17)
# -Wno-c++98-compat-pedantic: Disable C++98 compatibility pedantic warnings (target is C++17)
# -Wno-padded: Disable padding warnings
# Wno-exit-time-destructors: Disable exit-time destructors warnings
#add_compile_options(-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-exit-time-destructors)

# Source files
set(SOURCES
    src/app.cpp
    src/core/args.cpp
    src/core/log.cpp
    src/io/disk.cpp
    src/io/strings.cpp
    src/main.cpp
)

# Executable definition
add_executable(${PROJECT_NAME} ${SOURCES})

# Install DLLs in the same directory as the executable on Windows
if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# Link-Time Optimization (must be placed after add_executable)
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result AND NOT CMAKE_BUILD_TYPE MATCHES Debug)
    set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "Link Time Optimization is enabled.")
else()
    message(STATUS "Link Time Optimization is not supported or disabled for Debug build.")
endif()

# Generate compile_commands.json for better tooling support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Print the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
