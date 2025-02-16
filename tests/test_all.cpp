/**
 * @file test_all.cpp
 */

#include <algorithm>      // for std::sort
#include <cstddef>        // for std::size_t
#include <cstdlib>        // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>      // for std::exception
#include <filesystem>     // for std::filesystem
#include <fstream>        // for std::ofstream
#include <functional>     // for std::function
#include <stdexcept>      // for std::runtime_error
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

#include <fmt/core.h>
#include <fmt/ranges.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>         // for SetConsoleCP, SetConsoleOutputCP, CP_UTF8
#endif

#include "app.hpp"
#include "core/args.hpp"
#include "core/string.hpp"
#include "modules/analyze.hpp"

#include "examples.hpp"
#include "helpers.hpp"

#define TEST_EXECUTABLE_NAME "tests"

namespace test_args {
[[nodiscard]] int none();
[[nodiscard]] int invalid();
[[nodiscard]] int paths();
}  // namespace test_args

namespace test_analyze {
[[nodiscard]] int analyze_badly_formatted();
[[nodiscard]] int analyze_no_issues();
[[nodiscard]] int analyze_bare();
[[nodiscard]] int analyze_unused();
[[nodiscard]] int analyze_unlisted();
}  // namespace test_analyze

namespace test_app {
[[nodiscard]] int paths();
}  // namespace test_app

/**
 * @brief Entry-point of the test application.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 *
 * @return EXIT_SUCCESS if the test application ran successfully, EXIT_FAILURE otherwise.
 */
int main(int argc,
         char **argv)
{
#if defined(_WIN32)  // Setup UTF-8 input/output
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    // Define the formatted help message
    const std::string help_message = fmt::format(
        "Usage: {} <test>\n"
        "\n"
        "Run automatic tests.\n"
        "\n"
        "Positional arguments:\n"
        "  test  name of the test to run ('all' to run all tests)\n",
        argv[0]);

    // If no arguments, print help message and exit
    if (argc == 1) {
        fmt::print("{}\n", help_message);
        return EXIT_FAILURE;
    }

    // Otherwise, define argument to function mapping
    const std::unordered_map<std::string, std::function<int()>> tests = {
        {"test_args::none", test_args::none},
        {"test_args::invalid", test_args::invalid},
        {"test_args::paths", test_args::paths},
        {"test_analyze::analyze_badly_formatted", test_analyze::analyze_badly_formatted},
        {"test_analyze::analyze_no_issues", test_analyze::analyze_no_issues},
        {"test_analyze::analyze_bare", test_analyze::analyze_bare},
        {"test_analyze::analyze_unused", test_analyze::analyze_unused},
        {"test_analyze::analyze_unlisted", test_analyze::analyze_unlisted},
        {"test_app::paths", test_app::paths},
    };

    // Get the test name from the command-line arguments
    const std::string arg = argv[1];

    // If the test name is found, run the corresponding test
    if (const auto it = tests.find(arg); it != tests.cend()) {
        try {
            return it->second();
        }
        catch (const std::exception &e) {
            fmt::print(stderr, "Test '{}' threw an exception: {}\n", arg, e.what());
            return EXIT_FAILURE;
        }
    }
    else if (arg == "all") {
        // Run all tests sequentially and print the results
        bool all_passed = true;
        for (const auto &[name, test_func] : tests) {
            fmt::print("Running test: {}\n", name);
            try {
                const int result = test_func();
                if (result != EXIT_SUCCESS) {
                    all_passed = false;
                    fmt::print(stderr, "Test '{}' failed.\n", name);
                }
                else {
                    fmt::print("Test '{}' passed.\n", name);
                }
            }
            catch (const std::exception &e) {
                all_passed = false;
                fmt::print(stderr, "Test '{}' threw an exception: {}\n", name, e.what());
            }
        }
        return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else {
        fmt::print(stderr, "Error: Invalid test name: '{}'\n\n{}\n", arg, help_message);
        return EXIT_FAILURE;
    }
}

int test_args::none()
{
    try {
        char test_executable_name[] = TEST_EXECUTABLE_NAME;
        char *fake_argv[] = {test_executable_name};
        core::args::Args(1, fake_argv);
        fmt::print(stderr, "core::args::Args() failed: missing arguments were not caught.\n");
        return EXIT_FAILURE;
    }
    catch (const core::args::ArgsError &) {
        fmt::print("core::args::Args() passed: no arguments.\n");
        return EXIT_SUCCESS;
    }
}

int test_args::invalid()
{
    try {
        char test_executable_name[] = TEST_EXECUTABLE_NAME;
        char arg_hello[] = "hello";
        char *fake_argv[] = {test_executable_name, arg_hello};
        core::args::Args(2, fake_argv);
        // This should never be reached, as the ArgsError exception should be thrown by the constructor
        fmt::print(stderr, "core::args::Args() failed: invalid argument was not caught.\n");
        return EXIT_FAILURE;
    }
    catch (const core::args::ArgsError &e) {
        fmt::print("core::args::Args() passed: invalid argument caught: {:.40}\n", e.what());
        return EXIT_SUCCESS;
    }
}

int test_args::paths()
{
    try {
        // Create a temporary directory using RAII
        const helpers::TempDir temp_dir(std::filesystem::temp_directory_path() / TEST_EXECUTABLE_NAME);

        // Two dummy CPP files
        const auto temp_file1 = temp_dir.get() / "example1.cpp";
        const auto temp_file2 = temp_dir.get() / "example2.cpp";

        // Write to the files
        {
            std::ofstream ofs1(temp_file1);
            if (!ofs1) {
                throw std::runtime_error("Failed to open temp_file1 for writing");
            }
            ofs1 << examples::badly_formatted;

            std::ofstream ofs2(temp_file2);
            if (!ofs2) {
                throw std::runtime_error("Failed to open temp_file2 for writing");
            }
            ofs2 << examples::badly_formatted;
        }

        // Store the string representation of the directory path
        const std::string temp_dir_str = temp_dir.get().string();

        // Create mutable copies of the strings
        char test_executable_name[] = TEST_EXECUTABLE_NAME;

        // Create a mutable character array for temp_dir_str
        std::vector<char> temp_dir_cstr(temp_dir_str.cbegin(), temp_dir_str.cend());
        temp_dir_cstr.emplace_back('\0');  // Ensure null-termination

        // Build the argv array with mutable strings
        char *fake_argv[] = {test_executable_name, temp_dir_cstr.data()};
        const core::args::Args args(2, fake_argv);

        // Compare the filepaths found by Args
        if (args.filepaths.size() != 2) {
            fmt::print(stderr,
                       "Filepaths test failed: expected 2, got {}: {}\n",
                       args.filepaths.size(),
                       fmt::join(core::string::paths_to_strings(args.filepaths), ", "));
            return EXIT_FAILURE;
        }

        // Iterate, because the order is not guaranteed
        for (const auto &path : args.filepaths) {
            if (!std::filesystem::equivalent(path, temp_file1) && !std::filesystem::equivalent(path, temp_file2)) {
                fmt::print(stderr,
                           "Filepaths test failed: expected '{}' or '{}', got '{}'\n",
                           temp_file1.string(),
                           temp_file2.string(),
                           path.string());
                return EXIT_FAILURE;
            }
        }

        fmt::print("test_args::paths() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::args::Args() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_analyze::analyze_badly_formatted()
{
    try {
        // Create a temporary directory using RAII
        const helpers::TempDir temp_dir(std::filesystem::temp_directory_path() / TEST_EXECUTABLE_NAME);

        // Create a temporary file
        const auto temp_file = temp_dir.get() / "badly_formatted.cpp";
        {
            std::ofstream f(temp_file);
            if (!f) {
                throw std::runtime_error("Failed to open temp_file for writing");
            }
            f << examples::badly_formatted;
        }

        // Create expected results
        const std::vector<modules::analyze::BareInclude> expected_bare_includes = {
            modules::analyze::BareInclude(8, "#include <iostream>", "#include <iostream>"),
            modules::analyze::BareInclude(9, "        #INCLUDE <FMT/CORE.H>", "#include <fmt/core.h>"),
        };
        const std::vector<modules::analyze::IncludeWithUnusedFunctions> expected_unused_functions = {
            modules::analyze::IncludeWithUnusedFunctions(12, "#include <algorithm>  //     for std::find", {"std::find"}),
            modules::analyze::IncludeWithUnusedFunctions(15, "    #INCLUDE <ITERATOR>  // for std::back_inserter, std::transform", {"std::back_inserter", "std::transform"}),
        };
        const std::vector<modules::analyze::UnlistedFunction> expected_unlisted_functions = {
            modules::analyze::UnlistedFunction(35, "    STD::SORT(RESULT.BEGIN(), RESULT.END());", "std::sort", "https://duckduckgo.com/?sites=cppreference.com&q=std%3A%3Asort&ia=web"),
        };

        // Analyze the temporary file
        modules::analyze::CodeParser parser(temp_file);

        // Compare bare includes
        if (!helpers::compare_and_print_bare_includes(parser.get_bare_includes(), expected_bare_includes)) {
            throw std::runtime_error("Bare include test failed.");
        }

        // Compare unused functions
        if (!helpers::compare_and_print_unused_functions(parser.get_unused_functions(), expected_unused_functions)) {
            throw std::runtime_error("Unused functions test failed.");
        }

        // Compare unlisted functions
        if (!helpers::compare_and_print_unlisted_functions(parser.get_unlisted_functions(), expected_unlisted_functions)) {
            throw std::runtime_error("Unlisted functions test failed.");
        }

        fmt::print("test_analyze::analyze_badly_formatted() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "test_analyze::analyze_badly_formatted() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_analyze::analyze_no_issues()
{
    try {
        // Create a temporary directory using RAII
        const helpers::TempDir temp_dir(std::filesystem::temp_directory_path() / TEST_EXECUTABLE_NAME);

        // Create a temporary file
        const auto temp_file = temp_dir.get() / "no_issues.cpp";
        {
            std::ofstream f(temp_file);
            if (!f) {
                throw std::runtime_error("Failed to open temp_file for writing");
            }
            f << examples::no_issues;
        }

        // Create expected results
        const std::vector<modules::analyze::BareInclude> expected_bare_includes = {};
        const std::vector<modules::analyze::IncludeWithUnusedFunctions> expected_unused_functions = {};
        const std::vector<modules::analyze::UnlistedFunction> expected_unlisted_functions = {};

        // Analyze the temporary file
        modules::analyze::CodeParser parser(temp_file);

        // Compare bare includes
        if (!helpers::compare_and_print_bare_includes(parser.get_bare_includes(), expected_bare_includes)) {
            throw std::runtime_error("Bare include test failed.");
        }

        // Compare unused functions
        if (!helpers::compare_and_print_unused_functions(parser.get_unused_functions(), expected_unused_functions)) {
            throw std::runtime_error("Unused functions test failed.");
        }

        // Compare unlisted functions
        if (!helpers::compare_and_print_unlisted_functions(parser.get_unlisted_functions(), expected_unlisted_functions)) {
            throw std::runtime_error("Unlisted functions test failed.");
        }

        fmt::print("test_analyze::analyze_no_issues() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "test_analyze::analyze_no_issues() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_analyze::analyze_bare()
{
    try {
        // Create a temporary directory using RAII
        const helpers::TempDir temp_dir(std::filesystem::temp_directory_path() / TEST_EXECUTABLE_NAME);

        // Create a temporary file
        const auto temp_file = temp_dir.get() / "bare.cpp";
        {
            std::ofstream f(temp_file);
            if (!f) {
                throw std::runtime_error("Failed to open temp_file for writing");
            }
            f << examples::bare;
        }

        // Create expected results
        const std::vector<modules::analyze::BareInclude> expected_bare_includes = {
            modules::analyze::BareInclude(8, "    #include <fmt/core.h>", "#include <fmt/core.h>"),
            modules::analyze::BareInclude(9, "#include<pathmaster/pathmaster.hpp>", "#include<pathmaster/pathmaster.hpp>"),
        };
        const std::vector<modules::analyze::IncludeWithUnusedFunctions> expected_unused_functions = {};
        const std::vector<modules::analyze::UnlistedFunction> expected_unlisted_functions = {};

        // Analyze the temporary file
        modules::analyze::CodeParser parser(temp_file);

        // Compare bare includes
        if (!helpers::compare_and_print_bare_includes(parser.get_bare_includes(), expected_bare_includes)) {
            throw std::runtime_error("Bare include test failed.");
        }

        // Compare unused functions
        if (!helpers::compare_and_print_unused_functions(parser.get_unused_functions(), expected_unused_functions)) {
            throw std::runtime_error("Unused functions test failed.");
        }

        // Compare unlisted functions
        if (!helpers::compare_and_print_unlisted_functions(parser.get_unlisted_functions(), expected_unlisted_functions)) {
            throw std::runtime_error("Unlisted functions test failed.");
        }

        fmt::print("test_analyze::analyze_bare() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "test_analyze::analyze_bare() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_analyze::analyze_unused()
{
    try {
        // Create a temporary directory using RAII
        const helpers::TempDir temp_dir(std::filesystem::temp_directory_path() / TEST_EXECUTABLE_NAME);

        // Create a temporary file
        const auto temp_file = temp_dir.get() / "unused.cpp";
        {
            std::ofstream f(temp_file);
            if (!f) {
                throw std::runtime_error("Failed to open temp_file for writing");
            }
            f << examples::unused;
        }

        // Create expected results
        const std::vector<modules::analyze::BareInclude> expected_bare_includes = {};
        const std::vector<modules::analyze::IncludeWithUnusedFunctions> expected_unused_functions = {
            modules::analyze::IncludeWithUnusedFunctions(1, "  #include<string>//std::string,std::to_string", {"std::string", "std::to_string"}),
            modules::analyze::IncludeWithUnusedFunctions(3, "#INCLUDE <vector>//std::vector", {"std::vector"}),
            modules::analyze::IncludeWithUnusedFunctions(4, "#include <ALGORITHM>//for std::find, STD::TRANSFORM, std::back_inserter", {"std::find", "std::transform", "std::back_inserter"}),
            modules::analyze::IncludeWithUnusedFunctions(5, "#include <cstddef>        // for std::size_t,        std::nullptr_t", {"std::nullptr_t"}),
        };
        const std::vector<modules::analyze::UnlistedFunction> expected_unlisted_functions = {};

        // Analyze the temporary file
        modules::analyze::CodeParser parser(temp_file);

        // Compare bare includes
        if (!helpers::compare_and_print_bare_includes(parser.get_bare_includes(), expected_bare_includes)) {
            throw std::runtime_error("Bare include test failed.");
        }

        // Compare unused functions
        if (!helpers::compare_and_print_unused_functions(parser.get_unused_functions(), expected_unused_functions)) {
            throw std::runtime_error("Unused functions test failed.");
        }

        // Compare unlisted functions
        if (!helpers::compare_and_print_unlisted_functions(parser.get_unlisted_functions(), expected_unlisted_functions)) {
            throw std::runtime_error("Unlisted functions test failed.");
        }

        fmt::print("test_analyze::analyze_unused() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "test_analyze::analyze_unused() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_analyze::analyze_unlisted()
{
    try {
        // Create a temporary directory using RAII
        const helpers::TempDir temp_dir(std::filesystem::temp_directory_path() / TEST_EXECUTABLE_NAME);

        // Create a temporary file
        const auto temp_file = temp_dir.get() / "unlisted.cpp";
        {
            std::ofstream f(temp_file);
            if (!f) {
                throw std::runtime_error("Failed to open temp_file for writing");
            }
            f << examples::unlisted;
        }

        // Create expected results
        const std::vector<modules::analyze::BareInclude> expected_bare_includes = {};
        const std::vector<modules::analyze::IncludeWithUnusedFunctions> expected_unused_functions = {};
        const std::vector<modules::analyze::UnlistedFunction> expected_unlisted_functions = {
            modules::analyze::UnlistedFunction(3, "const std::size_t pi = 3.14159;", "std::size_t", "https://duckduckgo.com/?sites=cppreference.com&q=std%3A%3Asize_t&ia=web"),
            modules::analyze::UnlistedFunction(4, "std::sort(v.begin(), v.end());", "std::sort", "https://duckduckgo.com/?sites=cppreference.com&q=std%3A%3Asort&ia=web"),
        };

        // Analyze the temporary file
        modules::analyze::CodeParser parser(temp_file);

        // Compare bare includes
        if (!helpers::compare_and_print_bare_includes(parser.get_bare_includes(), expected_bare_includes)) {
            throw std::runtime_error("Bare include test failed.");
        }

        // Compare unused functions
        if (!helpers::compare_and_print_unused_functions(parser.get_unused_functions(), expected_unused_functions)) {
            throw std::runtime_error("Unused functions test failed.");
        }

        // Compare unlisted functions
        if (!helpers::compare_and_print_unlisted_functions(parser.get_unlisted_functions(), expected_unlisted_functions)) {
            throw std::runtime_error("Unlisted functions test failed.");
        }

        fmt::print("test_analyze::analyze_unlisted() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "test_analyze::analyze_unlisted() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_app::paths()
{
    try {
        // Create fake files and run the app

        // Create a temporary directory using RAII
        const helpers::TempDir temp_dir(std::filesystem::temp_directory_path() / TEST_EXECUTABLE_NAME);

        // Create a temporary file
        const auto temp_file = temp_dir.get() / "paths.cpp";
        {
            std::ofstream f(temp_file);
            if (!f) {
                throw std::runtime_error("Failed to open temp_file for writing");
            }
            f << examples::unlisted;
        }

        // Store the string representation of the file path
        const std::string temp_file_str = temp_file.string();

        // Create mutable copies of the strings
        char test_executable_name[] = TEST_EXECUTABLE_NAME;
        std::vector<char> temp_file_cstr(temp_file_str.cbegin(), temp_file_str.cend());
        temp_file_cstr.emplace_back('\0');  // Ensure null-termination

        char *fake_argv[] = {test_executable_name, temp_file_cstr.data()};
        app::run(core::args::Args(2, fake_argv));

        fmt::print("test_app::paths() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "test_app::paths() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}
