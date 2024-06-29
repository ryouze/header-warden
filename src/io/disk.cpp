/**
 * @file disk.cpp
 */

#include <algorithm>      // for std::transform, std::find
#include <cstddef>        // for std::size_t
#include <filesystem>     // for std::filesystem
#include <fstream>        // for std::ifstream
#include <iterator>       // for std::istreambuf_iterator
#include <iterator>       // for std::back_inserter
#include <regex>          // for std::regex, std::smatch, std::regex_search, std::sregex_iterator
#include <sstream>        // for std::istringstream
#include <stdexcept>      // for std::runtime_error, std::invalid_argument
#include <string>         // for std::string, std::getline, std::to_string
#include <unordered_set>  // for std::unordered_set
#include <vector>         // for std::vector

#include "../core/globals.hpp"
#include "../utils/log.hpp"
#include "../utils/strings.hpp"
#include "disk.hpp"

namespace {

/**
 * @brief Private helper function to load lines from a file.
 *
 * Each Line object contains the line number and text.
 *
 * @param file_path Path to the file to load (e.g., "src/main.cpp").
 *
 * @return Vector of Line objects, with each Line representing a single line in the file (e.g., "{Line(1, "#include <iostream>"), Line(2, "int main() {")").
 *
 * @throws std::invalid_argument If the file_path does not exist or is a directory.
 * @throws std::runtime_error If the file cannot be opened for reading or if any other I/O error occurs.
 *
 * @note The line numbers are 1-based.
 */
[[nodiscard]] std::vector<io::disk::Line> load_lines(const std::string &file_path)
{
    // Error: Doesn't exist
    if (!std::filesystem::exists(file_path)) {
        throw std::invalid_argument("File does not exist: " + file_path);
    }
    // Error: Is a directory
    else if (std::filesystem::is_directory(file_path)) {
        throw std::invalid_argument("Path is a directory, not a file: " + file_path);
    }

    // Open the file in read mode
    LOG_DEBUG("Loading file from disk: " + file_path);
    std::ifstream file(file_path);

    // Error: File cannot be opened
    if (!file) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    // Read the entire file into a string at once
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Split the content into lines, this is a bit more efficient than reading line by line
    std::vector<io::disk::Line> lines;
    std::istringstream stream(content);
    std::size_t line_number = 1;
    for (std::string line; std::getline(stream, line); ++line_number) {
        lines.emplace_back(line_number, line);
    }

    // Return shrunk vector of lines (RVO)
    lines.shrink_to_fit();
    return lines;
}

}  // namespace

io::disk::File::File(const std::string &file_path)
{
    // Define the regex for an include directive (e.g., "#include <iostream>") and a function call (e.g., "std::cout")
    static const std::regex include_directive_regex(R"(^\s*#include\s*<\S+>)");
    static const std::regex function_call_regex(R"(std::(\w+))");

    // Define containers for the different types of information
    std::vector<disk::IncludeWithFunctions> temp_includes_with_functions;
    std::vector<disk::Functions> temp_functions;

    // Load the file from disk and iterate over each line
    for (const Line &current_line : load_lines(file_path)) {

        // Print the original line, but operate on the modified line
        LOG_DEBUG("Processing line no. " + std::to_string(current_line.number) + ": " + current_line.text);

        // Skip if the raw line is empty (avoid stripping and lowercasing the line)
        if (current_line.text.empty()) {
            LOG_DEBUG("Skipping line, because the line is empty");
            continue;
        }

        // Strip leading and trailing whitespace from line, turn line lowercase
        std::string processed_line = utils::strings::to_lower(utils::strings::trim_whitespace(current_line.text));
        // LOG_DEBUG("Line after removing leading & trailing whitespace and turning it to lowercase: " + processed_line);

        // Skip if the processed line is a comment
        if (processed_line.at(0) == '*' || processed_line.compare(0, 2, "//") == 0) {
            LOG_DEBUG("Skipping line, because the line is a comment");
            continue;
        }

        // Check if the line contains an include directive and extract it
        std::string include_directive;
        std::smatch include_match;
        const bool line_contains_include = std::regex_search(processed_line, include_match, include_directive_regex);
        if (line_contains_include) {
            include_directive = include_match.str(0);
        }
        // If the line does not contain an include directive, remove comments to prevent false positives
        // E.g., "int x = 5; // Use std::cout to print it" -> "int x = 5;
        else {
            processed_line = utils::strings::remove_comments(processed_line);
        }

        // Get all std::function calls in the processed line
        std::vector<std::string> function_calls;
        std::sregex_iterator begin(processed_line.cbegin(), processed_line.cend(), function_call_regex), end;
        std::transform(begin, end, std::back_inserter(function_calls),
                       [](const std::smatch &match) {
                           return match.str(0);
                       });

        // Categorize the result into containers
        if (line_contains_include && !function_calls.empty()) {  // e.g., "#include <iostream> // for std::cout, std::cerr"

            // Verbose logging: Check if verbose mode is enabled, because  "vector_to_string" is expensive (LOG_DEBUG will check "core::globals::verbose" again, but it's faster than "vector_to_string")
            if (core::globals::verbose) {
                LOG_DEBUG("Verdict: Found include directive '" + include_directive + "' with the following associated functions: " + utils::strings::vector_to_string(function_calls));
            }
            temp_includes_with_functions.emplace_back(current_line, include_directive, function_calls);
        }
        else if (line_contains_include) {  // e.g., "#include <string>""
            LOG_DEBUG("Verdict: Found bare include directive: " + include_directive);

            // Extract bare includes (if enabled)
            if (core::globals::bare) {
                this->bare_includes_.emplace_back(current_line, include_directive);
            }
            else {
                LOG_DEBUG("Verdict: Not extracting bare includes, because the flag is disabled");
            }
        }
        else if (!function_calls.empty()) {  // e.g., "std::string"
            if (core::globals::verbose) {
                LOG_DEBUG("Verdict: Found functions: " + utils::strings::vector_to_string(function_calls));
            }
            temp_functions.emplace_back(current_line, function_calls);
        }
        else {
            LOG_DEBUG("Verdict: No include directive, functions, or comments found");
        }
    }

    // Extract unused functions (if enabled)
    if (core::globals::unused) {

        // For each include directive with associated functions (e.g., "#include <iostream> // for std::cout, std::cerr")
        for (const auto &include_with_associated_functions : temp_includes_with_functions) {

            // Initialize all functions in the include directive as not referenced
            std::vector<std::string> functions_not_referenced = include_with_associated_functions.functions;

            // For each function in the file (e.g., "std::cout")
            for (const auto &function_in_file : temp_functions) {

                // If the function is referenced in the include directive, remove it from the not referenced vector
                for (const auto &function_name : function_in_file.functions) {
                    const auto iterator_to_unreferenced_function = std::find(functions_not_referenced.cbegin(), functions_not_referenced.cend(), function_name);
                    if (iterator_to_unreferenced_function != functions_not_referenced.cend()) {
                        functions_not_referenced.erase(iterator_to_unreferenced_function);
                    }
                }
            }

            // If there are functions in the not referenced vector, add them to the unused functions vector
            if (!functions_not_referenced.empty()) {
                this->unused_functions_.emplace_back(include_with_associated_functions.line, include_with_associated_functions.name, functions_not_referenced);
            }
        }
    }
    else {
        LOG_DEBUG("Not extracting unused functions, because the flag is disabled");
    }

    // Extract missing functions (if enabled)
    if (core::globals::unlisted) {

        // Create a set of all functions in include directives
        std::unordered_set<std::string> functions_in_include_directives;

        // For each include directive with associated functions (e.g., "#include <iostream> // for std::cout, std::cerr")
        for (const auto &include_with_associated_functions : temp_includes_with_functions) {

            // Add all functions in include directives to the set
            functions_in_include_directives.insert(include_with_associated_functions.functions.cbegin(), include_with_associated_functions.functions.cend());
        }

        // For each function in the file (e.g., "std::cout")
        for (const auto &function_in_file : temp_functions) {

            // For each function name (string)
            for (const auto &function_name : function_in_file.functions) {

                // If the function is not in the set of functions in include directives, add it to the missing functions vector
                if (functions_in_include_directives.find(function_name) == functions_in_include_directives.cend()) {
                    this->missing_functions_.emplace_back(function_in_file.line, std::vector<std::string>{function_name});
                }
            }
        }
    }
    else {
        LOG_DEBUG("Not extracting missing functions, because the flag is disabled");
    }
}

const std::vector<io::disk::BareInclude> &io::disk::File::get_bare_includes() const
{
    return this->bare_includes_;
}

const std::vector<io::disk::IncludeWithFunctions> &io::disk::File::get_unused_functions() const
{
    return this->unused_functions_;
}

const std::vector<io::disk::Functions> &io::disk::File::get_missing_functions() const
{
    return this->missing_functions_;
}
