/**
 * @file disk.hpp
 *
 * @brief Disk I/O functions.
 */

#pragma once

#include "core/log.hpp"
#include "strings.hpp"

#include <algorithm>
#include <cstddef>     // for std::size_t
#include <filesystem>  // for std::filesystem
#include <fstream>     // for std::ifstream
#include <iterator>    // for std::istreambuf_iterator
#include <regex>
#include <sstream>    // for std::istringstream
#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string, std::getline, std::to_string
#include <vector>     // for std::vector

namespace disk {

/**
 * @brief Struct that represents a single line in a file.
 */
struct Line {

    /**
     * @brief Construct a new Line object.
     *
     * @param number Line number (e.g., "5").
     * @param text Line text (e.g., "std::string foo()").
     */
    explicit Line(const size_t number, const std::string &text)
        : number(number), text(text) {}

    /**
     * @brief Line number (e.g., "5").
     */
    const size_t number;

    /**
     * @brief Line text (e.g., "std::string foo()").
     */
    const std::string text;
};

/**
 * @brief Struct that represents a bare include directive in a file.
 */
struct BareInclude {

    /**
     * @brief Construct a new BareInclude object.
     *
     * @param line Line where the include directive is found.
     * @param name Name of the included file (e.g., "#include <iostream>").
     */
    BareInclude(const Line &line, const std::string &name)
        : line(line), name(name) {}

    /**
     * @brief Line where the include directive is found.
     */
    const Line line;

    /**
     * @brief Name of the included file (e.g., "#include <iostream>").
     */
    const std::string name;
};

/**
 * @brief Struct that represents an include directive with associated functions in a file.
 */
struct IncludeWithFunctions {

    /**
     * @brief Construct a new IncludeWithFunctions object.
     *
     * @param line Line where the include directive is found.
     * @param name Name of the included file (e.g., "#include <iostream>").
     * @param functions Functions associated with the include directive (e.g., "std::cout").
     */
    IncludeWithFunctions(const Line &line, const std::string &name, const std::vector<std::string> &functions)
        : line(line), name(name), functions(functions) {}

    /**
     * @brief Line where the include directive is found.
     */
    const Line line;

    /**
     * @brief Name of the included file (e.g., "#include <iostream>").
     */
    const std::string name;

    /**
     * @brief Functions associated with the include directive (e.g., "std::cout").
     */
    const std::vector<std::string> functions;
};

/**
 * @brief Struct that represents a free-standing function not in an include directive in a file.
 */
struct Functions {

    /**
     * @brief Construct a new Functions object.
     *
     * @param line Line where the function is found.
     * @param functions Functions found on the line (e.g., "std::cout").
     */
    Functions(const Line &line, const std::vector<std::string> &functions)
        : line(line), functions(functions) {}

    /**
     * @brief Line where the function is found.
     */
    const Line line;

    /**
     * @brief Functions found on the line.
     */
    const std::vector<std::string> functions;
};

/**
 * @brief Class that extracts information from a file.
 */
class File {
  public:
    explicit File(const std::string &file_path)
    {
        // Load the file from disk and iterate over each line
        for (const Line &line : this->load_lines(file_path)) {

            // The trick here is that we print the original line, but we actually operate on the modified line
            LOG_DEBUG("Processing line no. " + std::to_string(line.number) + ": " + line.text);

            // Strip leading and trailing whitespace from line, turn line lowercase
            std::string modified_line = strings::to_lower(strings::trim_whitespace(line.text));
            LOG_DEBUG("Line after removing leading & trailing whitespace and turning it to lowercase: " + modified_line);

            // Skip if the modified line is a comment
            if (modified_line.find("//") == 0 || modified_line.find("*") == 0) {
                LOG_DEBUG("Skipping line, because the line is a comment");
                continue;
            }
            // Skip if the modified line is empty
            else if (modified_line.empty()) {
                LOG_DEBUG("Skipping line, because the line is empty");
                continue;
            }
            // Otherwise, process it with regex
            else {
                // Extracted the include directive regex as a constant to avoid recompilation
                static const std::regex include_directive_regex(R"(#include\s*<\S+>)");

                // Extracted the function call regex as a constant to avoid recompilation
                static const std::regex function_call_regex(R"(std::(\w+))");

                // 1) Check if the line contains an include directive
                std::smatch include_match;
                const bool line_contains_include = std::regex_search(modified_line, include_match, include_directive_regex);

                if (line_contains_include) {
                    const std::string include_directive = include_match.str(0);
                    LOG_INFO("Found include directive '" + include_directive + "'");
                }

                // 2) Get all std::function calls in the modified line
                std::vector<std::string> function_matches;

                // If line_contains_include is false, we remove comments to prevent false positives
                if (!line_contains_include) {
                    modified_line = strings::remove_comments(modified_line);
                }

                // Create const iterators pointing to the start and end of the sequence of matches
                std::sregex_iterator begin(modified_line.cbegin(), modified_line.cend(), function_call_regex), end;

                // Use std::transform to iterate over each match in the sequence
                std::transform(begin, end, std::back_inserter(function_matches),
                               [](const std::smatch &match) {
                                   return match.str(0);
                               });

                if (function_matches.empty()) {
                    LOG_INFO("VERDICT: No std::function calls found in the line");
                }
                else {
                    LOG_INFO("VERDICT: Found the following std::function calls: " + strings::vector_to_string(function_matches));
                }

                // Categorize the result into containers
                if (line_contains_include && !function_matches.empty()) {  // #include <iostream> // for std::cout, std::cerr
                    this->includes_with_functions.emplace_back(line, include_match.str(0), function_matches);
                }
                else if (line_contains_include) {  // #include <string>
                    this->bare_includes.emplace_back(line, include_match.str(0));
                }
                else if (!function_matches.empty()) {  // std::string
                    this->functions.emplace_back(line, function_matches);
                }
            }
        }
    }

    const std::vector<disk::BareInclude> &get_bare_includes() const
    {
        return bare_includes;
    }

    const std::vector<disk::IncludeWithFunctions> &get_includes_with_functions() const
    {
        return includes_with_functions;
    }

    const std::vector<disk::Functions> &get_functions() const
    {
        return functions;
    }

  private:
    /**
   * @brief Load lines from a file and return them as a vector of Line objects.
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
    std::vector<Line> load_lines(const std::string &file_path)
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
        LOG_INFO("Loading file from disk: " + file_path);
        std::ifstream file(file_path);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }

        // Read the entire file into a string at once
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        // Split the content into lines, this is a bit more efficient than reading line by line
        std::vector<Line> lines;
        std::size_t line_number = 1;
        std::istringstream stream(content);
        for (std::string buffer; std::getline(stream, buffer); ++line_number) {
            lines.emplace_back(line_number, buffer);
        }

        // Return the lines
        return lines;
    }

    std::vector<disk::BareInclude> bare_includes;
    std::vector<disk::IncludeWithFunctions> includes_with_functions;
    std::vector<disk::Functions> functions;
};

}  // namespace disk
