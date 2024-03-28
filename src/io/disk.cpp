/**
 * @file disk.hpp
 */

#include "disk.hpp"
#include "../core/log.hpp"
#include "strings.hpp"

#include <algorithm>      // for std::transform
#include <algorithm>      // for std::find
#include <cstddef>        // for std::size_t
#include <filesystem>     // for std::filesystem
#include <fstream>        // for std::ifstream
#include <iterator>       // for std::istreambuf_iterator
#include <iterator>       // for std::back_inserter
#include <regex>          // for std::regex, std::smatch, std::regex_search, std::sregex_iterator
#include <sstream>        // for std::istringstream
#include <stdexcept>      // for std::runtime_error
#include <string>         // for std::string, std::getline, std::to_string
#include <unordered_set>  // for std::unordered_set
#include <vector>         // for std::vector

namespace {

/**
* @brief Private helper function to load lines from a file.
*
* Each Line object contains the line number and text.
*
* @param file_path Path to the file to load (e.g., "src/main.cpp").
*
* @return Vector of Line objects, with each Line representing a single line in the file (e.g., "{Line(1, "#include <iostream>"), Line(2, "int main() {")").

* @throws std::runtime_error If the file_path does not exist, is not a regular file, or if there was an error opening the file.
*
* @note The line numbers are 1-based.
*/
[[nodiscard]] std::vector<io::disk::Line> load_lines(const std::string &file_path)
{
    // Throw if the file does not exist
    if (!std::filesystem::exists(file_path)) {
        throw std::runtime_error("File does not exist: " + file_path);
    }

    // Throw if the path is not a file
    if (!std::filesystem::is_regular_file(file_path)) {
        throw std::runtime_error("Path is not a file: " + file_path);
    }

    // Load a file from disk
    LOG_DEBUG("Loading file from disk: " + file_path);
    std::ifstream file(file_path);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    // Read the entire file into a string at once
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Split the content into lines, this is a bit more efficient than reading line by line
    std::vector<io::disk::Line> lines;
    std::size_t line_number = 1;
    std::istringstream stream(content);
    for (std::string buffer; std::getline(stream, buffer); ++line_number) {
        lines.emplace_back(line_number, buffer);
    }

    // Return the lines (RVO)
    return lines;
}
}  // namespace

io::disk::File::File(const std::string &file_path)
{
    // Define the regex for an include directive (e.g., "#include <iostream>") and a function call (e.g., "std::cout")
    static const std::regex include_directive_regex(R"(#include\s*<\S+>)");
    static const std::regex function_call_regex(R"(std::(\w+))");

    // Define containers for the different types of information
    std::vector<disk::IncludeWithFunctions> temp_includes_with_functions;
    std::vector<disk::Functions> temp_functions;

    // Load the file from disk and iterate over each line
    for (const Line &current_line : load_lines(file_path)) {

        // Print the original line, but operate on the modified line
        LOG_DEBUG("Processing line no. " + std::to_string(current_line.number) + ": " + current_line.text);

        // Strip leading and trailing whitespace from line, turn line lowercase
        std::string processed_line = strings::to_lower(strings::trim_whitespace(current_line.text));
        // LOG_DEBUG("Line after removing leading & trailing whitespace and turning it to lowercase: " + processed_line);

        // Skip if the processed line is a comment or empty
        if (processed_line.find("//") == 0 || processed_line.find("*") == 0 || processed_line.empty()) {
            LOG_DEBUG("Skipping line, because the line is a comment or empty");
            continue;
        }

        // Check if the line contains an include directive and extract it
        std::string include_directive;
        std::smatch include_match;
        const bool line_contains_include = std::regex_search(processed_line, include_match, include_directive_regex);
        if (line_contains_include) {
            include_directive = include_match.str(0);
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
            LOG_DEBUG("Found include directive with associated functions");
            temp_includes_with_functions.emplace_back(current_line, include_directive, function_calls);
        }
        else if (line_contains_include) {  // e.g., "#include <string>""
            LOG_DEBUG("Found bare include directive");
            this->bare_includes.emplace_back(current_line, include_directive);
        }
        else if (!function_calls.empty()) {  // e.g., "std::string"
            LOG_DEBUG("Found functions");
            temp_functions.emplace_back(current_line, function_calls);
        }
    }

    // Extract unused functions
    for (const auto &include_with_associated_functions : temp_includes_with_functions) {
        std::vector<std::string> functions_not_referenced = include_with_associated_functions.functions;
        for (const auto &function_in_file : temp_functions) {
            for (const auto &function_name : function_in_file.functions) {
                const auto iterator_to_unreferenced_function = std::find(functions_not_referenced.cbegin(), functions_not_referenced.cend(), function_name);
                if (iterator_to_unreferenced_function != functions_not_referenced.cend()) {
                    functions_not_referenced.erase(iterator_to_unreferenced_function);
                }
            }
        }
        if (!functions_not_referenced.empty()) {
            this->unused_functions.push_back({include_with_associated_functions.line, include_with_associated_functions.name, functions_not_referenced});
        }
    }

    // Extract missing functions
    std::unordered_set<std::string> functions_in_include_directives;
    for (const auto &include_with_associated_functions : temp_includes_with_functions) {
        const std::vector<std::string> &functions_in_include = include_with_associated_functions.functions;
        functions_in_include_directives.reserve(functions_in_include.size());
        functions_in_include_directives.insert(include_with_associated_functions.functions.cbegin(), include_with_associated_functions.functions.cend());
    }
    for (const auto &function_in_file : temp_functions) {
        for (const auto &function_name : function_in_file.functions) {
            if (functions_in_include_directives.find(function_name) == functions_in_include_directives.cend()) {
                this->missing_functions.push_back({function_in_file.line, {function_name}});
            }
        }
    }
}

const std::vector<io::disk::BareInclude> &io::disk::File::get_bare_includes() const
{
    return this->bare_includes;
}

const std::vector<io::disk::IncludeWithFunctions> &io::disk::File::get_unused_functions() const
{
    return this->unused_functions;
}

const std::vector<io::disk::Functions> &io::disk::File::get_missing_functions() const
{
    return this->missing_functions;
}
