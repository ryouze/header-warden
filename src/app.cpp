/**
 * @file app.cpp
 */

#include "app.hpp"
#include "core/log.hpp"
#include "io/disk.hpp"
#include "io/strings.hpp"

#include <sstream>  // for std::ostringstream
#include <string>   // for std::string

std::string app::get_report(const std::string &file_path)
{
    static const std::string line_separator(80, '-');

    LOG_DEBUG("Processing file: " + file_path);
    const io::disk::File processed_file(file_path);

    std::stringstream report_message;
    report_message << "##- " << file_path << " -##\n\n";

    // Process bare includes
    report_message << "-- 1) BARE INCLUDES --\n\n";
    const auto &bare_includes = processed_file.get_bare_includes();
    if (bare_includes.empty()) {
        report_message << "-> No bare includes found.\n\n";
    }
    else {
        for (const auto &bare_include : bare_includes) {
            report_message << bare_include.line.number << "| " << bare_include.line.text << '\n'
                           << "-> Bare include directive.\n"
                           << "-> Add a comment to '" << bare_include.name << "' that lists which functions depend on it, e.g., '" << bare_include.name << "' // for std::foo, std::bar'.\n\n";
        }
    }

    // Process unused functions
    report_message << "-- 2) UNUSED FUNCTIONS --\n\n";
    const auto &unused_functions = processed_file.get_unused_functions();
    if (unused_functions.empty()) {
        report_message << "-> No unused functions found.\n\n";
    }
    else {
        for (const auto &unused_function : unused_functions) {
            report_message << unused_function.line.number << "| " << unused_function.line.text << '\n'
                           << "-> Unused functions listed as comments.\n"
                           << "-> Remove the following functions from comments of the '" << unused_function.name << "' include directive: " << io::strings::vector_to_string(unused_function.functions) << "\n\n";
        }
    }

    // Process missing functions
    report_message << "-- 3) UNLISTED FUNCTIONS --\n\n";
    const auto &missing_functions = processed_file.get_missing_functions();
    if (missing_functions.empty()) {
        report_message << "-> No unlisted functions found.\n\n";
    }
    else {
        for (const auto &missing_function : missing_functions) {
            for (const auto &function : missing_function.functions) {
                report_message << missing_function.line.number << "| " << missing_function.line.text << '\n'
                               << "-> Unlisted function.\n"
                               << "-> Add '" << function << "' as a comment to the include directives, e.g., \"#include <foo> // for " << function << "\"\n"
                               << "-> Reference: " << io::strings::create_cpp_reference_link(function) << "\n\n";
            }
        }
    }

    report_message << line_separator;

    return report_message.str();
}
