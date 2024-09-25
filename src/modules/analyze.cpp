/**
 * @file analyze.cpp
 */

#include <algorithm>      // for std::transform
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
    // TODO: Find a way to skip multiline comments in a smarter way
    return
        // Single-line comment (e.g., "// This is a comment")
        line.compare(0, 2, "//") == 0 ||
        // Multiline comment start (e.g., "/* This is a comment")
        line.compare(0, 2, "/*") == 0 ||
        // Multiline comment middle or end (e.g., "* This is a comment")
        // This will potentially match lines that are not comments (e.g., multiplication symbol as first character), causing false positives
        (!line.empty() && line.at(0) == '*');
}

}  // namespace

CodeParser::CodeParser(const std::filesystem::path &input_path)
{
    // Regular expression to match include directives, e.g., "#include <iostream>"
    static const std::regex include_directive_regex(R"(^\s*#include\s*<\S+>)", std::regex::optimize);
    // Regular expression to match any std:: identifier, e.g., "std::cout"
    static const std::regex std_identifier_regex(R"(std::(\w+))", std::regex::optimize);

    // Temporary containers to store parsed data
    std::vector<IncludeWithUnusedFunctions> temp_includes_with_functions;  // Include directives with listed functions
    std::vector<UnlistedFunction> temp_std_entities;                       // All std:: identifiers used in the code

    // Load the file from disk and iterate over each line
    for (const auto &[line_number, line_text] : core::io::read_lines(input_path)) {

        // Skip if the raw line is empty (avoid unnecessary processing)
        if (line_text.empty()) {
            continue;
        }

        // Strip leading and trailing whitespace and convert to lowercase
        std::string processed_line = core::string::to_lower(core::string::strip_whitespace(line_text));

        // Skip the line if it begins with a comment
        if (begins_with_comment(processed_line)) {
            continue;
        }

        // Variables to hold match results
        std::string include_directive;
        std::smatch include_match;

        // Check if the line contains an include directive
        const bool line_contains_include = std::regex_search(processed_line, include_match, include_directive_regex);
        if (line_contains_include) {
            // Extract the include directive (e.g., "#include <iostream>")
            include_directive = include_match.str(0);
        }
        else {
            // If not an include directive, remove inline comments to prevent false positives
            // E.g., "int x = 5; // Use std::cout to print it" becomes "int x = 5;", so we don't match "std::cout" later
            processed_line = core::string::remove_comment(processed_line);
        }

        // Find all std:: identifiers in the processed line
        std::vector<std::string> std_identifiers;
        std::sregex_iterator begin(processed_line.cbegin(), processed_line.cend(), std_identifier_regex), end;
        std::transform(begin, end, std::back_inserter(std_identifiers),
                       [](const std::smatch &match) { return match.str(0); });

        // Categorize the line based on its content
        if (line_contains_include && !std_identifiers.empty()) {
            // Line is an include directive with std:: identifiers in comments
            // E.g., "#include <iostream> // for std::cout, std::cerr"
            temp_includes_with_functions.emplace_back(line_number, line_text, std_identifiers);
        }
        else if (line_contains_include) {
            // Line is an include directive without any std:: identifiers
            // E.g., "#include <string>"
            this->bare_includes_.emplace_back(line_number, line_text, include_directive);
        }
        else if (!std_identifiers.empty()) {
            // Line contains std:: identifiers used in the code
            // E.g., identifier "std::string" in line "std::string name;".
            for (const auto &identifier_name : std_identifiers) {
                // Do not create C++ reference links yet, we''ll do that later
                temp_std_entities.emplace_back(line_number, line_text, identifier_name, "");
            }
        }
        // Lines that don't match any of the above are ignored
        // E.g., '#include "my_header.hpp"'
    }

    // --- EXTRACT UNUSED FUNCTIONS ---
    // Create a set of all std:: identifiers used in the code for quick lookup
    std::unordered_set<std::string> used_std_identifiers;
    for (const auto &entity_in_file : temp_std_entities) {
        used_std_identifiers.insert(entity_in_file.function);
    }

    // Identify unused functions listed in include directives
    for (const auto &include_with_functions : temp_includes_with_functions) {

        // Initialize a vector to hold functions listed in the include directive but not used in the code
        std::vector<std::string> functions_not_referenced;

        // Check each function listed in the include directive
        for (const auto &func : include_with_functions.unused_functions) {
            if (used_std_identifiers.find(func) == used_std_identifiers.cend()) {
                // Function is listed but not used; add it to the list
                functions_not_referenced.emplace_back(func);
            }
        }

        // If there are any unused functions, add them to the unused_functions_ vector
        if (!functions_not_referenced.empty()) {
            this->unused_functions_.emplace_back(include_with_functions.number, include_with_functions.text, functions_not_referenced);
        }
    }

    // --- EXTRACT MISSING FUNCTIONS ---
    // Create a set of all functions listed in include directives
    std::unordered_set<std::string> functions_in_include_directives;
    for (const auto &include_with_functions : temp_includes_with_functions) {
        functions_in_include_directives.insert(include_with_functions.unused_functions.cbegin(), include_with_functions.unused_functions.cend());
    }

    // Identify functions used in the code but not listed in any include directive's comments
    for (const auto &entity_in_file : temp_std_entities) {
        if (functions_in_include_directives.find(entity_in_file.function) == functions_in_include_directives.cend()) {
            // Function is used but not listed; add it to the unlisted_functions_ vector
            // Also, create a link to the C++ reference for the function
            this->unlisted_functions_.emplace_back(entity_in_file.number, entity_in_file.text, entity_in_file.function,
                                                   core::string::create_cpp_reference_link(entity_in_file.function));
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
