# Minimum required version of CMake (3.19 is the first version to support Apple Silicon)
cmake_minimum_required(VERSION 3.19)

# Project name
project(header-warden
    VERSION 1.0
    DESCRIPTION "Find missing standard library headers in C++."
    LANGUAGES CXX
)

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory (mkdir build && cd build && cmake ..). Feel free to remove CMakeCache.txt and CMakeFiles.")
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

# Set optimization levels for different configurations:
# - Release: Enable level 3 optimizations (-O3) for maximum performance
# - MinSizeRel: Optimize for size (-Os), enabling all -O2 optimizations that do not increase code size
add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
add_compile_options("$<$<CONFIG:MINSIZEREL>:-Os>")

# Compile options (must be placed before add_executable)
# -Wall: Enable all warnings
# -Wextra: Enable extra warnings that are not enabled by -Wall
# -Werror: Treat warnings as errors
# -Wpedantic: Request strict ISO C++ compliance
add_compile_options(-Wall -Wextra -Werror -Wpedantic)

# Source files
set(SOURCES
    src/args.cpp
    src/disk.cpp
    src/main.cpp
    src/net.cpp
)

# Executable definition
add_executable(${PROJECT_NAME} ${SOURCES})

# Link-Time Optimization (must be placed after add_executable)
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set_property(GLOBAL
        PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE
    )
    message(STATUS "Link Time Optimization is enabled.")
else()
    message(STATUS "Link Time Optimization is not supported.")
endif()

# Print the release type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
