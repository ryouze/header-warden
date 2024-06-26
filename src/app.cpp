/**
 * @file app.cpp
 */

#include <exception>  // for std::exception
#include <sstream>    // for std::ostringstream
#include <string>     // for std::string
#include <vector>     // for std::vector

#include "app.hpp"
#include "core/globals.hpp"
#include "io/disk.hpp"
#include "utils/log.hpp"
#include "utils/strings.hpp"

namespace {

/**
 * @brief Private helper function to get a report of issues found in a file.
 *
 * @param file_path Path to the file to process (e.g., "src/app.hpp").
 *
 * @return Report for the file. This is a formatted string that lists any issues found in the file.
 */
[[nodiscard]] std::string get_report(const std::string &file_path)
{
    LOG_DEBUG("Processing file: " + file_path);

    static const std::string line_separator(80, '-');
    bool has_issues = false;
    std::ostringstream report_message;

    // Load the file from disk and process it on construction
    const io::disk::File processed_file(file_path);

    // Process bare includes if enabled
    if (core::globals::bare) {
        const auto &bare_includes = processed_file.get_bare_includes();
        if (!bare_includes.empty()) {
            has_issues = true;
            report_message << "-- 1) BARE INCLUDES --\n\n";
            for (const auto &bare_include : bare_includes) {
                report_message << bare_include.line.number << "| " << bare_include.line.text << '\n'
                               << "-> Bare include directive.\n"
                               << "-> Add a comment to '" << bare_include.name << "' that lists which functions depend on it, e.g., '" << bare_include.name << "' // for std::foo, std::bar'.\n\n";
            }
        }
    }

    // Process unused functions if enabled
    if (core::globals::unused) {
        const auto &unused_functions = processed_file.get_unused_functions();
        if (!unused_functions.empty()) {
            has_issues = true;
            report_message << "-- 2) UNUSED FUNCTIONS --\n\n";
            for (const auto &unused_function : unused_functions) {
                report_message << unused_function.line.number << "| " << unused_function.line.text << '\n'
                               << "-> Unused functions listed as comments.\n"
                               << "-> Remove the following functions from comments of the '" << unused_function.name << "' include directive: " << utils::strings::vector_to_string(unused_function.functions) << "\n\n";
            }
        }
    }

    // Process missing functions if enabled
    if (core::globals::unlisted) {
        const auto &missing_functions = processed_file.get_missing_functions();
        if (!missing_functions.empty()) {
            has_issues = true;
            report_message << "-- 3) UNLISTED FUNCTIONS --\n\n";
            for (const auto &missing_function : missing_functions) {
                for (const auto &function : missing_function.functions) {
                    report_message << missing_function.line.number << "| " << missing_function.line.text << '\n'
                                   << "-> Unlisted function.\n"
                                   << "-> Add '" << function << "' as a comment to the include directives, e.g., \"#include <foo> // for " << function << "\"\n"
                                   << "-> Reference: " << utils::strings::create_cpp_reference_link(function) << "\n\n";
                }
            }
        }
    }

    // If no issues were found, add an OK message
    if (!has_issues) {
        report_message << "-> OK.\n\n";
    }

    report_message << line_separator;

    return "##- " + file_path + " -##\n\n" + report_message.str();
}

}  // namespace

void app::run(const std::vector<std::string> &file_paths)
{
    // Iterate over each file path and log the report
    for (const auto &file_path : file_paths) {
        try {
            LOG_INFO(get_report(file_path));
        }
        // If an exception is thrown, log the error and continue to the next file
        catch (const std::exception &e) {
            LOG_WARNING("Ignoring invalid path '" + file_path + "': " + std::string(e.what()));
        }
    }
}
