#include "disk.hpp"  // for disk::LineType, disk::Line, disk::File

#include <algorithm>  // for std::transform
#include <cctype>     // for std::tolower
#include <cstddef>    // for std::size_t
#include <fstream>    // for std::ifstream
#include <iterator>   // for std::back_inserter
#include <regex>      // for std::regex, std::regex_search, std::smatch, std::sregex_iterator
#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string, std::getline
#include <vector>     // for std::vector

namespace disk {

// Helper functions that are not available outside this file
namespace {

/**
 * @brief Convert a string to lowercase.
 *
 * This is a helper function that is not available outside this file.
 *
 * @param str String to turn lowercase (e.g., "Hello WORLD").
 *
 * @return Lowercased string (e.g., "hello world").
 */
[[nodiscard]] std::string convert_str_to_lowercase(const std::string &str)
{
    // Create copy of the input string (so we can use RVO when returning the result)
    std::string lower_str = str;
    // Convert the string to lowercase
    std::transform(lower_str.cbegin(), lower_str.cend(), lower_str.begin(),
                   // std::to_lower expects an int, so we cast the char to an unsigned char
                   [](unsigned char c) { return std::tolower(c); });
    return lower_str;
}

}  // namespace

Line::Line(const std::size_t line_number,
           const std::string &line_text)
    : number_(line_number), text_(line_text), type_(LineType::EMPTY)
{
    // Ignore empty lines
    if (this->text_.empty()) {
        return;
    }
    // Ignore comments (line starts with "*" or "//", optionally preceded by whitespace)
    if (this->contains_regex(std::regex(R"(^\s*\*)")) || this->contains_regex(std::regex(R"(^\s*//.*)"))) {
        return;
    }
    // Extract all standard library functions used in the line
    this->functions_ = this->get_all_regex_matches(
        std::regex(R"(std::(\w+))"),  // Match standard library functions
        1);                           // First capture group (e.g., "string" from "std::string")
    // Extract the header name from the include directive in the line
    this->include_ = this->get_first_regex_match(
        std::regex(R"(^\s*?#include.?<(\S+)>)"),  // Match include directives
        1);                                       // First capture group (e.g., "iostream" from "#include <iostream>")
    // Classify the line based on its content
    if (!this->include_.empty()) {
        // If the line contains an include directive...
        if (!this->functions_.empty()) {
            // ...and also contains standard library functions, classify it as an include line with functions
            this->type_ = LineType::INCLUDE_WITH_FUNCTION;
        }
        else {
            // ...and does not contain standard library functions, classify it as an include line
            this->type_ = LineType::BARE_INCLUDE;
        }
    }
    else if (!this->functions_.empty()) {
        // If the line does not contain an include directive but contains standard library functions, classify it as a function line
        this->type_ = LineType::FUNCTION;
    }
    // If the line does not contain an include directive or standard library functions, it remains with the default type (EMPTY)
}

std::size_t Line::get_number() const
{
    return this->number_;
}

const std::string &Line::get_text() const
{
    return this->text_;
}

LineType Line::get_type() const
{
    return this->type_;
}

const std::vector<std::string> &Line::get_functions() const
{
    return this->functions_;
}

const std::string &Line::get_include() const
{
    return this->include_;
}

bool Line::contains_regex(const std::regex &regex_pattern) const
{
    // Use std::regex_search to check if the string contains the regex pattern
    return std::regex_search(this->text_, regex_pattern);
}

std::string Line::get_first_regex_match(const std::regex &regex_pattern,
                                        const std::size_t capture_group) const
{
    std::smatch match;
    // Return early if the regex search does not find a match or if the match size is not greater than the capture group.
    if (!std::regex_search(this->text_, match, regex_pattern) || match.size() <= capture_group) {
        return "";
    }
    return match.str(capture_group);
}

std::vector<std::string> Line::get_all_regex_matches(const std::regex &regex_pattern,
                                                     const std::size_t capture_group) const
{
    std::vector<std::string> result;
    // Create const iterators pointing to the start and end of the sequence of matches
    std::sregex_iterator begin(this->text_.cbegin(), this->text_.cend(), regex_pattern), end;
    // Use std::transform to iterate over each match in the sequence
    std::transform(begin, end, std::back_inserter(result),
                   [capture_group](const std::smatch &match) {
                       return match.size() > capture_group ? match.str(capture_group) : match.str();
                   });
    result.shrink_to_fit();
    return result;
}

File::File(const std::string &file_path,
           const bool turn_lowercase)
{
    // Load a file from disk
    std::ifstream file(file_path);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }
    // Count the number of lines from 1
    std::size_t line_number = 1;
    std::string buffer;
    while (std::getline(file, buffer)) {
        // If turn_lowercase is true, turn line's text lowercase
        this->lines_.emplace_back(line_number, turn_lowercase ? convert_str_to_lowercase(buffer) : buffer);
        ++line_number;
    }
    this->lines_.shrink_to_fit();
}

const std::vector<Line> &File::get_lines() const
{
    return this->lines_;
}

}  // namespace disk
