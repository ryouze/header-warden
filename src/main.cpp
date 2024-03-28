#include "core/args.hpp"
#include "core/globals.hpp"
#include "core/log.hpp"
#include "io/disk.hpp"
#include "io/strings.hpp"

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

        // Define a line separator for console output (outside loop for efficiency)
        static const std::string line_separator(80, '-');

        // Parse command line arguments
        const core::args::ArgParser arg_parser(argc, argv);

        // If no arguments are provided or help is requested, display help message
        if (arg_parser.empty() || arg_parser.contains("-h", "--help")) {
            std::cout << arg_parser.get_help();
            std::exit(EXIT_SUCCESS);
        }

        // If "-v" or "--verbose" argument was passed, set the global verbose flag to true
        core::globals::verbose = arg_parser.contains("-v", "--verbose");

        // Iterate over each file provided as a positional argument
        // This ignores arguments that start with a hyphen (e.g., "-h", "--help", "-v", "--verbose")
        for (const auto &file_path : arg_parser.get_positional_arguments()) {

            // Load the file from disk and extract information
            const io::disk::File processed_file(file_path);

            std::string message = "##- " + file_path + " -##\n\n";

            const auto &bare_includes = processed_file.get_bare_includes();

            if (!bare_includes.empty()) {

                message += "-- 1) BARE INCLUDES --\n\n";

                for (const auto &bare_include : bare_includes) {

                    message += std::to_string(bare_include.line.number) + "| " + bare_include.line.text + '\n' +
                               "-> Bare include directive.\n" +
                               "-> Add a comment to '" + bare_include.name + "' that lists which functions depend on it, e.g., '" + bare_include.name + "' // for std::foo, std::bar'.";
                    message += "\n\n";
                }
            }
            else {
                message += "-- 1) BARE INCLUDES --\n\n";
                message += "No bare includes found.\n\n";
            }

            const auto &unused_functions = processed_file.get_unused_functions();

            if (!unused_functions.empty()) {

                message += "-- 2) UNUSED FUNCTIONS --\n\n";

                for (const auto &unused_function : processed_file.get_unused_functions()) {

                    message += std::to_string(unused_function.line.number) + "| " + unused_function.line.text + '\n' +
                               "-> Unused functions listed as comments.\n" +
                               "-> Remove the following functions from comments of the '" + unused_function.name + "' include directive: " + io::strings::vector_to_string(unused_function.functions);
                    message += "\n\n";
                }
            }
            else {
                message += "-- 2) UNUSED FUNCTIONS --\n\n";
                message += "No unused functions found.\n\n";
            }

            const auto &missing_functions = processed_file.get_missing_functions();

            if (!missing_functions.empty()) {
                message += "-- 3) UNLISTED FUNCTIONS --\n\n";

                for (const auto &missing_function : processed_file.get_missing_functions()) {

                    for (const auto &function : missing_function.functions) {
                        message += std::to_string(missing_function.line.number) + "| " + missing_function.line.text + '\n' +
                                   "-> Unlisted function.\n" +
                                   "-> Add '" + function + "' as a comment to the include directives, e.g., \"#include <foo> // for " + function + "\"\n" +
                                   "-> Reference: " + io::strings::create_cpp_reference_link(function) + "\n\n";
                    }
                }
            }
            else {
                message += "-- 3) UNLISTED FUNCTIONS --\n\n";
                message += "No unlisted functions found.\n\n";
            }

            message += line_separator;

            LOG_INFO(message);
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
