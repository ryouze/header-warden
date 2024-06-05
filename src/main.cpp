/**
 * @file main.cpp
 */

#include <cstdlib>    // for std::exit, EXIT_FAILURE
#include <exception>  // for std::exception
#include <ios>        // for std::ios_base
#include <iostream>   // for std::cout
#include <string>     // for std::string, std::to_string
#include <vector>     // for std::vector

#include "app.hpp"
#include "core/args.hpp"
#include "core/globals.hpp"
#include "utils/log.hpp"

/**
 * @brief Entry-point of the application.
 */
int main(int argc, char **argv)
{
    try {
        // Disable synchronization between the C++ standard streams (e.g., std::cin, std::cout) and their corresponding C equivalents
        std::ios_base::sync_with_stdio(false);

        // Initialize vector of files to process
        std::vector<std::string> files;

        // Process args inside a separate scope to automatically deallocate parser's copy of the args
        {
            // Initialize command-line arguments parser
            const core::args::ArgParser parser(argc, argv);

            // If no arguments are provided or help is requested, display help message
            if (parser.empty() || parser.contains("-h", "--help")) {
                std::cout << parser.get_help();
                std::exit(EXIT_SUCCESS);
            }

            // Get positional arguments (i.e., arguments that are not preceded by a hyphen)
            files = parser.get_positional_arguments();

            // If empty, display help message
            if (files.empty()) {
                std::cout << parser.get_help();
                std::exit(EXIT_FAILURE);
            }

            // If "-v" or "--verbose" argument was passed, set the global verbose flag to true
            core::globals::verbose = parser.contains("-v", "--verbose");

            // If "--disable-bare" argument was passed, disable bare includes
            if (parser.contains("--disable-bare")) {
                core::globals::bare = false;
            }

            // If "--disable-unused" argument was passed, disable unused functions
            if (parser.contains("--disable-unused")) {
                core::globals::unused = false;
            }

            // If "--disable-unlisted" argument was passed, disable unlisted functions
            if (parser.contains("--disable-unlisted")) {
                core::globals::unlisted = false;
            }
        }

        LOG_DEBUG("Processing " + std::to_string(files.size()) + " file(s)");

        // Get report for each file provided as a positional argument
        // This ignores arguments that start with a hyphen (e.g., "-h", "--help", "-v", "--verbose")
        app::run(files);
    }
    catch (const std::exception &e) {
        LOG_ERROR(std::string(e.what()));
        std::exit(EXIT_FAILURE);
    }
    catch (...) {
        LOG_ERROR("Unknown error");
        std::exit(EXIT_FAILURE);
    }
    return 0;
}
