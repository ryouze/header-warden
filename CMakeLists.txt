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
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE
        "${default_build_type}"
        CACHE
        STRING "Choose the type of build."
        FORCE
    )
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS
        "Debug"
        "Release"
        "MinSizeRel"
        "RelWithDebInfo"
    )
endif()

# Set optimization levels for all targets based on build type:
# - Release: Enable level 3 optimizations (-O3) for maximum performance
# - MinSizeRel: Optimize for size (-Os), enabling all -O2 optimizations that do not increase code size
add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
add_compile_options("$<$<CONFIG:MINSIZEREL>:-Os>")

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
#add_compile_options(-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded)

# Source files
set(SOURCES
    src/args.cpp
    src/disk.cpp
    src/main.cpp
    src/net.cpp
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
if(result AND NOT CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "Link Time Optimization is enabled.")
else()
    message(STATUS "Link Time Optimization is either not supported by the compiler or has been manually disabled for Debug or RelWithDebInfo build types.")
endif()

# Print the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
