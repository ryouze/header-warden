/**
 * @file main.cpp
 */

#include "app.hpp"
#include "core/args.hpp"
#include "core/globals.hpp"
#include "core/log.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_set>

/**
 * @brief Entry-point of the application.
 */
int main(int argc, char **argv)
{
    try {
        // Disable synchronization between the C++ standard streams (e.g., std::cin, std::cout) and their corresponding C equivalents
        std::ios_base::sync_with_stdio(false);

        // Parse command line arguments
        const core::args::ArgParser arg_parser(argc, argv);

        // If no arguments are provided or help is requested, display help report_message
        if (arg_parser.empty() || arg_parser.contains("-h", "--help")) {
            std::cout << arg_parser.get_help();
            std::exit(EXIT_SUCCESS);
        }

        // If "-v" or "--verbose" argument was passed, set the global verbose flag to true
        core::globals::verbose = arg_parser.contains("-v", "--verbose");

        // Iterate over each file provided as a positional argument
        // This ignores arguments that start with a hyphen (e.g., "-h", "--help", "-v", "--verbose")
        for (const auto &file_path : arg_parser.get_positional_arguments()) {
            LOG_INFO(app::get_report(file_path));
        }

        // Keep processing the next file until all files are processed
    }
    catch (const std::exception &e) {
        LOG_ERROR(e.what());
        std::exit(EXIT_FAILURE);
    }
    catch (...) {
        LOG_ERROR("Unknown error");
        std::exit(EXIT_FAILURE);
    }
    return 0;
}
