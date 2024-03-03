#include "disk.hpp"

#include <algorithm>  // for std::transform
#include <cctype>     // for std::tolower
#include <cstddef>    // for std::size_t
#include <fstream>    // for std::ifstream
#include <iterator>   // for std::back_inserter
#include <regex>      // for std::regex, std::regex_search, std::smatch, std::sregex_iterator
#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string, std::getline
#include <vector>     // for std::vector

// Helper functions that are not available outside this file
namespace {

/**
 * @brief Remove comments from a string.
 *
 * This is a helper function that is not available outside this file.
 *
 * @param str String from which to remove comments (e.g., "int x = 5 // My comment").
 *
 * @return String without comments (e.g., "int x = 5").
 */
[[nodiscard]] std::string remove_comments_from_str(const std::string &str)
{
    // Create a copy of the input string (so we can use RVO when returning the result)
    std::string temp = str;
    // Find index of "//" in the string
    if (const auto index = str.find("//"); index != std::string_view::npos) {
        // Remove everything from the index to the end of the string
        temp.erase(index);
    }
    return temp;
}

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

disk::Line::Line(const std::size_t line_number,
                 const std::string &line_text)
    : number_(line_number), text_(line_text), type_(disk::LineType::EMPTY)
{
    // Ignore empty lines
    if (this->text_.empty()) {
        return;
    }
    try {
        // Ignore comments (line starts with "*" or "//", optionally preceded by whitespace)
        if (this->contains_regex(std::regex(R"(^\s*\*)")) || this->contains_regex(std::regex(R"(^\s*//.*)"))) {
            return;
        }
        // Extract header name from include directive, if present
        this->include_ = this->get_first_regex_match(std::regex(R"(^\s*?#include.?<(\S+)>)"), 1);
        const bool line_contains_include = !this->include_.empty();
        // Extract all standard library functions used in the line
        // If line contains include directive, comments are not removed to preserve functions listed after the directive
        // E.g., "#include <iostream> // for std::cout, for std::cerr"
        // If line does not contain include directive, comments are removed to prevent false positives
        // E.g., "int x = 5 // use std::cout to print x to the console"
        this->functions_ = this->get_all_regex_matches(std::regex(R"(std::(\w+))"), 1, !line_contains_include);
        const bool line_contains_functions = !this->functions_.empty();
        // Classify the line based on its content
        if (line_contains_include) {
            this->type_ = line_contains_functions ? LineType::INCLUDE_WITH_FUNCTION : LineType::BARE_INCLUDE;
        }
        else if (line_contains_functions) {
            this->type_ = LineType::FUNCTION;
        }
        // If no include directive or functions, line remains with default type (EMPTY)
    }
    catch (const std::exception &e) {
        throw std::runtime_error("Failed to process line " + std::to_string(line_number) + " ('" + this->text_ + "'): " + e.what());
    }
}

std::size_t disk::Line::get_number() const
{
    return this->number_;
}

const std::string &disk::Line::get_text() const
{
    return this->text_;
}

disk::LineType disk::Line::get_type() const
{
    return this->type_;
}

const std::vector<std::string> &disk::Line::get_functions() const
{
    return this->functions_;
}

const std::string &disk::Line::get_include() const
{
    return this->include_;
}

bool disk::Line::contains_regex(const std::regex &regex_pattern) const
{
    // Use std::regex_search to check if the string contains the regex pattern
    return std::regex_search(this->text_, regex_pattern);
}

std::string disk::Line::get_first_regex_match(const std::regex &regex_pattern,
                                              const std::size_t capture_group) const
{
    std::smatch match;
    // Return early if the regex search does not find a match or if the match size is not greater than the capture group.
    if (!std::regex_search(this->text_, match, regex_pattern) || match.size() <= capture_group) {
        return "";
    }
    return match.str(capture_group);
}

std::vector<std::string> disk::Line::get_all_regex_matches(const std::regex &regex_pattern,
                                                           const std::size_t capture_group,
                                                           const bool remove_comments) const
{
    // If remove_comments is true, remove comments from the line
    const std::string temp = remove_comments ? remove_comments_from_str(this->text_) : this->text_;
    std::vector<std::string> result;
    // Create const iterators pointing to the start and end of the sequence of matches
    std::sregex_iterator begin(temp.cbegin(), temp.cend(), regex_pattern), end;
    // Use std::transform to iterate over each match in the sequence
    std::transform(begin, end, std::back_inserter(result),
                   [capture_group](const std::smatch &match) {
                       return match.size() > capture_group ? match.str(capture_group) : match.str();
                   });
    return result;
}

disk::File::File(const std::string &file_path,
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

const std::vector<disk::Line> &disk::File::get_lines() const
{
    return this->lines_;
}
