# Set minimum required version of CMake (3.20.1 properly supports Apple Silicon)
cmake_minimum_required(VERSION 3.20.1)

# Set project name and language
project(
  header-warden
  LANGUAGES CXX
)

# Set standard without compiler specific extensions, export compile commands
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Require out-of-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a separate build directory and run CMake from there.")
endif()

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to \"Release\" as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Enable IPO for Release builds (if supported)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    message(STATUS "IPO enabled.")
  else()
    message(STATUS "IPO disabled for non-release builds.")
  endif()
else()
  message(WARNING "IPO not supported: ${ipo_error}")
endif()

# Enable ccache (if available, install using "brew install ccache")
find_program(CCACHE ccache)
if(CCACHE)
  message(STATUS "Ccache enabled.")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else()
  message(WARNING "Ccache not found. Consider installing it for faster rebuilds.")
endif()

# Add executable target
add_executable(${PROJECT_NAME}
  src/app.cpp
  src/core/args.cpp
  src/io/disk.cpp
  src/main.cpp
  src/utils/log.cpp
  src/utils/strings.cpp
)

# Enable compile warnings (if not on Windows)
if(NOT WIN32)
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall                 # Enable most warning flags
    -Wcast-align          # Warn for potential performance problems when casting a pointer to a type with stricter alignment requirements
    -Wconversion          # Warn on type conversions that may lose data
    -Wdouble-promotion    # Warn if a float is implicitly promoted to double
    -Werror               # Treat warnings as errors (stop compilation if any warning is present)
    -Wextra               # Enable extra warning flags that are not enabled by "-Wall"
    -Wformat=2            # Warn on security issues around functions that format output (like printf), this includes all "-Wformat" warnings and more
    -Wnon-virtual-dtor    # Warn when a class with virtual functions has a non-virtual destructor, which can lead to undefined behavior
    -Wnull-dereference    # Warn if a null dereference is detected
    -Wold-style-cast      # Warn for usage of C-style casts
    -Woverloaded-virtual  # Warn when a derived class function declaration may be an error due to hiding a virtual function from the base class
    -Wpedantic            # Warn on features that are not part of the ISO C++ standard
    -Wshadow              # Warn when a local variable shadows another local variable, parameter, global variable, or function
    -Wsign-conversion     # Warn on sign conversions (e.g., when a negative integer is converted to an unsigned integer)
    -Wunused              # Warn on anything being unused
  )
  # Enable all compile warnings (if using Clang or Apple Clang)
  # if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  #   # Some warnings are disabled because they are not useful (e.g., C++98 compatibility warnings)
  #   target_compile_options(${PROJECT_NAME} PRIVATE
  #     -Weverything                # Enable all warnings (literally all of them)
  #     -Wno-c++98-compat           # Disable C++98 compatibility warnings
  #     -Wno-c++98-compat-pedantic  # Disable C++98 compatibility pedantic warnings
  #     -Wno-exit-time-destructors  # Disable exit-time destructors warnings
  #   )
  #   message(STATUS "All warnings enabled.")
  # endif()
endif()

# Add install target
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Print the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}.")
