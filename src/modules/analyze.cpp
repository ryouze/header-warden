/**
 * @file analyze.cpp
 */

#include <algorithm>      // for std::transform, std::find
#include <filesystem>     // for std::filesystem
#include <iterator>       // for std::back_inserter
#include <regex>          // for std::regex, std::smatch, std::sregex_iterator, std::regex_search
#include <string>         // for std::string
#include <unordered_set>  // for std::unordered_set
#include <vector>         // for std::vector

#include "analyze.hpp"
#include "core/io.hpp"
#include "core/string.hpp"

namespace modules::analyze {

namespace {

/**
 * @brief Private helper function to check if a line begins with a comment.
 *
 * @param line Line to check (e.g., "// This is a comment").
 *
 * @return True if the line begins with a comment, false otherwise.
 *
 * @note The string is expected to be stripped of leading whitespace.
 */
[[nodiscard]] bool begins_with_comment(const std::string &line)
{
    return
        // Comment line (e.g., "// This is a comment")
        line.compare(0, 2, "//") == 0 ||
        // Multiline comment start (e.g., "/* This is a comment")
        line.compare(0, 2, "/*") == 0 ||
        // Multiline comment middle or end (e.g., "* This is a comment")
        // This will potentially match lines that are not comments (e.g., multiplication on newline)
        // TODO: Find a way to skip multiline comments in a smarter way
        line.at(0) == '*';
}

}  // namespace

CodeParser::CodeParser(const std::filesystem::path &input_path)
{
    // Define the regex for an include directive (e.g., "#include <iostream>") and a function call (e.g., "std::cout")
    static const std::regex include_directive_regex(R"(^\s*#include\s*<\S+>)", std::regex::optimize);
    static const std::regex function_call_regex(R"(std::(\w+))", std::regex::optimize);

    // Define containers for the different types of information
    std::vector<IncludeWithUnusedFunctions> temp_includes_with_functions;
    std::vector<UnlistedFunction> temp_functions;

    // Load the file from disk and iterate over each line
    for (const auto &[line_number, line_text] : core::io::read_lines(input_path)) {

        // Skip if the raw line is empty (avoid stripping and lowercasing the line)
        if (line_text.empty()) {
            continue;
        }

        // Strip leading and trailing whitespace from line, turn line lowercase
        std::string processed_line = core::string::to_lower(core::string::strip_whitespace(line_text));

        // Skip if the processed line is a comment
        if (begins_with_comment(processed_line)) {
            continue;
        }

        // Check if the line contains an include directive
        std::string include_directive;
        std::smatch include_match;
        const bool line_contains_include = std::regex_search(processed_line, include_match, include_directive_regex);
        if (line_contains_include) {
            // If it does, extract the include directive (e.g., "#include <iostream>")
            include_directive = include_match.str(0);
        }
        else {
            // If the line does not contain an include directive, remove comments to prevent false positives
            // E.g., "int x = 5; // Use std::cout to print it" -> "int x = 5;
            processed_line = core::string::remove_comment(processed_line);
        }

        // Get all std::function calls in the processed line
        std::vector<std::string> function_calls;
        std::sregex_iterator begin(processed_line.cbegin(), processed_line.cend(), function_call_regex), end;
        std::transform(begin, end, std::back_inserter(function_calls),
                       [](const std::smatch &match) { return match.str(0); });

        // Categorize the result into containers
        if (line_contains_include && !function_calls.empty()) {
            // E.g., "#include <iostream> // for std::cout, std::cerr"
            temp_includes_with_functions.emplace_back(line_number, line_text, function_calls);
        }
        else if (line_contains_include) {
            // E.g., "#include <string>""
            this->bare_includes_.emplace_back(line_number, line_text, include_directive);
        }
        else if (!function_calls.empty()) {
            // E.g., "std::string"
            for (const auto &function_name : function_calls) {
                // Do not create C++ reference links, we do this later
                temp_functions.emplace_back(line_number, line_text, function_name, "");
            }
        }
        // Otherwise, the line is ignored, e.g., '#include "my_header.hpp"'
    }

    // Extract unused functions
    for (const auto &include_with_associated_functions : temp_includes_with_functions) {

        // Initialize all functions in the include directive as not referenced
        std::vector<std::string> functions_not_referenced = include_with_associated_functions.unused_functions;

        // For each function in the file (e.g., "std::cout")
        for (const auto &function_in_file : temp_functions) {
            // If the function is referenced in the include directive, remove it from the not referenced vector
            const auto iterator_to_unreferenced_function = std::find(functions_not_referenced.cbegin(), functions_not_referenced.cend(), function_in_file.function);
            if (iterator_to_unreferenced_function != functions_not_referenced.cend()) {
                functions_not_referenced.erase(iterator_to_unreferenced_function);
            }
        }
        // If there are functions in the not referenced vector, add them to the unused functions vector
        if (!functions_not_referenced.empty()) {
            this->unused_functions_.emplace_back(include_with_associated_functions.number, include_with_associated_functions.text, functions_not_referenced);
        }
    }

    // Extract missing functions
    std::unordered_set<std::string> functions_in_include_directives;

    // For each include directive with associated functions (e.g., "#include <iostream> // for std::cout, std::cerr")
    for (const auto &include_with_associated_functions : temp_includes_with_functions) {
        // Add all functions in include directives to the set
        functions_in_include_directives.insert(include_with_associated_functions.unused_functions.cbegin(), include_with_associated_functions.unused_functions.cend());
    }

    // For each function in the file (e.g., "std::cout")
    for (const auto &function_in_file : temp_functions) {
        // If the function is not in the set of functions in include directives, add it to the missing functions vector
        // Also, create a link to the C++ reference for the function
        if (functions_in_include_directives.find(function_in_file.function) == functions_in_include_directives.cend()) {
            this->unlisted_functions_.emplace_back(function_in_file.number, function_in_file.text, function_in_file.function,
                                                   core::string::create_cpp_reference_link(function_in_file.function));
        }
    }
}

const std::vector<BareInclude> &CodeParser::get_bare_includes() const
{
    return this->bare_includes_;
}

const std::vector<IncludeWithUnusedFunctions> &CodeParser::get_unused_functions() const
{
    return this->unused_functions_;
}

const std::vector<UnlistedFunction> &CodeParser::get_unlisted_functions() const
{
    return this->unlisted_functions_;
}

}  // namespace modules::analyze
