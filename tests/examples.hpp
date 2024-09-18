/**
 * @file examples.hpp
 *
 * @brief Code examples to analyze.
 */

#pragma once

#include <string_view>  // for std::string_view

namespace examples {

inline constexpr std::string_view badly_formatted = R"(/**
 * @file example.hpp
 *
 * @brief Example of a badly formatted file.
 */

// Bare include: This include does not have any accompanying comments
#include <iostream>
        #INCLUDE <FMT/CORE.H>

// Unused functions listed as comments: This include has a function (std::find) that is not actually used within the file
#include <algorithm>  //     for std::find

// Unused functions listed as comments: This include has multiple functions (std::transform, std::back_inserter) that are not actually used within the file
    #INCLUDE <ITERATOR>  // for std::back_inserter, std::transform

// OK: This include is correctly documented with the used functions listed in the comments
#include <string>  // for std::string,    std::to_string
#include <vector>  // for   std::vector

// OK: This function uses "std::string" and "std::to_string" from the <string> header and "std::vector" from the <vector> header
std::vector<STD::STRING> foo(const std::vector<int> &vec)
{
    std::vector<std::string> result;
    for (const auto &i : vec) {
        result.emplace_back(std::to_string(i));
    }
    return result;
}

// Unlisted function: This function uses "std::sort", but it's not listed in the comments after the includes
std::vector<int> bar(const std::vector<int> &v)
{
    std::vector<int> result(v);
    STD::SORT(RESULT.BEGIN(), RESULT.END());
    return result;
}
)";

inline constexpr std::string_view no_issues = R"(/**
 * @file shell.hpp
 *
 * @brief Run shell commands.
 */

#pragma once

#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string

namespace core::shell {

/**
 * @brief Base class for exceptions raised by the command builder.
 */
class PathError : public std::runtime_error {
  public:
    explicit PathError(const std::string &message)
        : std::runtime_error(message) {}
};

/**
 * @brief Build the command to open the filepath in the default web browser (or any other default application for a given file type).
 *
 * The following commands are returned:
 * - macOS: "open"
 * - GNU/Linux: "xdg-open"
 * - Windows: "start"
 *
 * The filepath is wrapped in quotes before being concatenated with the platform-specific command.
 *
 * @return Command to open the default web browser (e.g., open "~/data.html").
 *
 * @throws PathError If failed to build the command or the platform is not supported.
 */
[[nodiscard]] std::string build_command(const std::string &filepath);

// Now let's see if comments are ignored.
// The output can be printed using std::cout.
/**
 * The output can be printed using std::cout.
 * The output can be printed using std::cout.
 * The output can be printed using std::cout.
 */

}  // namespace core::shell)";

inline constexpr std::string_view bare = R"(/**
 * @file args.cpp
 */

// Extra spaces
    #include <string>        //STD::STRING

    #include <fmt/core.h>
#include<pathmaster/pathmaster.hpp>

    #include "args.hpp"
    #include "version.hpp"

core::args::Args::Args(const int argc,
                       char **argv)
{
    // If no arguments, do nothing
    if (argc == 1) {
        return;
    }
    // If one or more arguments
    else {
        // Define the formatted help message
        const std::string help_message =
            "Usage: yt-table [-h] [-v]\n"
            "\n"
            "Manage YouTube subscriptions locally through a shell-like interface.\n"
            "\n"
            "Optional arguments:\n"
            "  -h, --help     prints help message and exits\n"
            "  -v, --version  prints version and exits\n";

        // Get the first argument as a string
        const std::string arg = argv[1];

        if (arg == "-h" || arg == "--help") {
            // If "-h" or "--help" is passed as the first argument, throw ArgsMessage with the help message
            throw ArgsMessage(help_message);
        }
        else if (arg == "-v" || arg == "--version") {
            // If "-v" or "--version" is passed as the first argument, throw ArgsMessage with the version
            throw ArgsMessage(fmt::format("{}", PROJECT_VERSION));
        }
        else {
            // Otherwise, throw ArgsError with the help message
            throw ArgsError(fmt::format("Error: Invalid argument: {}\n\n{}", arg, help_message));
        }
    }
})";

inline constexpr std::string_view unused = R"(  #include<string>//std::string,std::to_string
#INCLUDE <IOSTREAM>      //     STD::COUT
#INCLUDE <vector>//std::vector
#include <ALGORITHM>//for std::find, STD::TRANSFORM, std::back_inserter
#include <cstddef>        // for std::size_t,        std::nullptr_t

const std::size_t pi = 3.14159;
std::cout << "Hello world!\n";)";

inline constexpr std::string_view unlisted = R"(#include <iostream>  // for std::cout
// #include <cstddef>  // for std::size_t
const std::size_t pi = 3.14159;
std::sort(v.begin(), v.end());
std::cout << "Hello world!\n";)";

}  // namespace examples
