#include "args.hpp"
#include "disk.hpp"
#include "net.hpp"

#include <algorithm>      // for std::find
#include <cstdlib>        // for std::exit, EXIT_SUCCESS, EXIT_FAILURE
#include <exception>      // for std::exception
#include <iostream>       // for std::cout, std::cerr
#include <string>         // for std::string, std::to_string
#include <unordered_set>  // for std::unordered_set
#include <vector>         // for std::vector

/**
 * @brief Entry-point of the application.
 */
int main(int argc, char **argv)
{
    try {
        // Disable synchronization between the C++ standard streams (e.g., std::cin, std::cout) and their corresponding C equivalents
        std::ios_base::sync_with_stdio(false);
        // Define a line separator for console output (outside loop for efficiency)
        const std::string line_separator(80, '-');
        // Parse command line arguments
        const args::ArgParser arg_parser(argc, argv);
        // If no arguments are provided or help is requested, display help message
        if (arg_parser.empty() || arg_parser.contains("-h", "--help")) {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]... [FILE]...\n\n"
                      << "Description:\n"
                      << "  Find missing standard library headers in C++ files.\n\n"
                      << "Options:\n"
                      << "  -h, --help       Display this help message and exit.\n"
                      << "  -v, --verbose    Display detailed output.\n\n"
                      << "Arguments:\n"
                      << "  FILE             One or more C++ files to analyze. Specify the path to each file.\n\n"
                      << "Example:\n"
                      << "  " << argv[0] << " main.cpp\n";
            std::exit(EXIT_SUCCESS);
        }
        // Check if verbose flag was set by user using "-v" or "--verbose"
        const bool is_verbose = arg_parser.contains("-v", "--verbose");
        // Define a lambda function for printing based on the verbose flag
        // If is_verbose is true, print the message, otherwise do nothing
        const auto conditional_verbose_log = [&is_verbose](const std::string &message) {
            if (is_verbose) {
                std::cout << message;
            }
        };
        // Iterate over each file provided as a positional argument
        // This ignores arguments that start with a hyphen (e.g., -h, --help, -v, --verbose)
        for (const auto &file_path : arg_parser.get_positional_arguments()) {
            // Read the file from disk
            conditional_verbose_log(line_separator + "\n##- " + file_path + ": SOURCE CODE -##\n\n");
            const disk::File source_file(file_path);
            // Initialize vectors to store different types of lines
            std::vector<disk::Line> lines_with_includes;                // Line with an include directive (e.g., "#include <string>")
            std::vector<disk::Line> lines_with_includes_and_functions;  // Line with an include directive and standard library functions (e.g., "#include <iostream> // for std::cout, std::cerr")
            std::vector<disk::Line> lines_with_functions;               // Line with a standard library function (e.g., "std::string")
            // Iterate over each line in the file
            for (const auto &line : source_file.get_lines()) {
                // Log the line number and text
                conditional_verbose_log(std::to_string(line.get_number()) + "| " + line.get_text() + '\n');
                // Put the line into the appropriate vector based on its type
                switch (line.get_type()) {
                case disk::LineType::BARE_INCLUDE:
                    conditional_verbose_log("-> Bare include directive.\n");
                    lines_with_includes.push_back(line);
                    break;
                case disk::LineType::INCLUDE_WITH_FUNCTION:
                    conditional_verbose_log("-> Include directive with listed functions as a comment.\n");
                    lines_with_includes_and_functions.push_back(line);
                    break;
                case disk::LineType::FUNCTION:
                    conditional_verbose_log("-> Standard library functions.\n");
                    lines_with_functions.push_back(line);
                    break;
                case disk::LineType::EMPTY:
                    conditional_verbose_log("-> Nothing.\n");
                    break;
                }
            }
            conditional_verbose_log("\n");
            // +-------------------------+ //
            // | ANALYZE & PRINT RESULTS | //
            // +-------------------------+ //
            // 1. Find bare include directives without listed functions
            // E.g., "#include <string>"
            std::cout << line_separator << "\n##- " << file_path << ": BARE INCLUDES -##\n\n";
            for (const auto &header_line : lines_with_includes) {
                // Process include directives without listed functions
                std::cout << header_line.get_number() << "| " + header_line.get_text() << '\n';
                std::cout << "-> Bare include directive.\n-> Add a comment to \"#include <" << header_line.get_include() << ">\" that lists which functions depend on it, e.g., \"#include <" << header_line.get_include() << "> // for std::foo, std::bar\".\n\n";
            }
            // 2. Find unused functions listed as comments in the include directives
            // E.g., "#include <iostream> // for std::cout, std::cerr"
            std::cout << line_separator << "\n##- " << file_path << ": UNUSED FUNCTIONS -##\n\n";
            for (const auto &include_line : lines_with_includes_and_functions) {
                // Process include directives with listed functions as a comment
                std::vector<std::string> unreferenced_functions = include_line.get_functions();
                for (const auto &function_line : lines_with_functions) {
                    for (const auto &function_name : function_line.get_functions()) {
                        const auto it = std::find(unreferenced_functions.cbegin(), unreferenced_functions.cend(), function_name);
                        if (it != unreferenced_functions.cend()) {
                            unreferenced_functions.erase(it);
                        }
                    }
                }
                if (!unreferenced_functions.empty()) {
                    std::cout << include_line.get_number() << "| " + include_line.get_text() << '\n';
                    std::cout << "-> Unused functions listed as comments.\n-> Remove the following functions from comments of the \"#include <" << include_line.get_include() << ">\" include directive: ";
                    for (const auto &unused_function : unreferenced_functions) {
                        std::cout << "\"std::" << unused_function << "\", ";
                    }
                    std::cout << "\b\b.\n\n";
                }
            }
            // 3. Find functions that are not listed in the include directives
            // E.g., "std::string"
            std::cout << line_separator << "\n##- " << file_path << ": UNLISTED FUNCTIONS -##\n\n";
            // Container for the names of included functions in the include directives
            std::unordered_set<std::string> referenced_functions;
            // Iterate over each line that has an include directive with listed functions
            for (const auto &include_line : lines_with_includes_and_functions) {
                // Get the list of functions from the include line
                const std::vector<std::string> &functions = include_line.get_functions();
                // Reserve space in the unordered set for the functions
                referenced_functions.reserve(functions.size());
                // Insert the functions into the unordered set
                referenced_functions.insert(include_line.get_functions().cbegin(), include_line.get_functions().cend());
            }
            // Iterate over each line that has a function
            for (const auto &function_line : lines_with_functions) {
                // Iterate over each function in the line
                for (const auto &function_name : function_line.get_functions()) {
                    // If the function is not in the unordered set of included functions
                    if (referenced_functions.find(function_name) == referenced_functions.cend()) {
                        std::cout << function_line.get_number() << "| " + function_line.get_text() << '\n';
                        std::cout << "-> Unlisted function.\n-> Add \"std::" << function_name << "\" as a comment to the include directives, e.g., \"#include <foo> // for std::" << function_name << "\".\n";
                        // Create a URL to cppreference.com for the function
                        std::cout << "-> Reference: " << net::create_cpp_reference_link("std::" + function_name) << "\n\n";
                    }
                }
            }
        }
        // Keep processing the next file until all files are processed
    }
    catch (const std::exception &e) {
        std::cerr << "ERROR: " << e.what() << '\n';
        std::exit(EXIT_FAILURE);
    }
    catch (...) {
        std::cerr << "ERROR: Unknown\n";
        std::exit(EXIT_FAILURE);
    }
    return 0;
}
