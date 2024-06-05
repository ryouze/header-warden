/**
 * @file strings.cpp
 */

#include <algorithm>      // for std::transform
#include <cctype>         // for std::tolower
#include <sstream>        // for std::ostringstream
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map

#include "../utils/log.hpp"
#include "strings.hpp"

std::string utils::strings::trim_whitespace(const std::string &str)
{
    LOG_DEBUG("Trimming whitespace from string '" + str + '\'');

    // Find the first non-whitespace character in the string
    const auto start_pos = str.find_first_not_of(" \t\n\r\f\v");

    // If the string is all whitespace, return an empty string
    if (start_pos == std::string::npos) {
        LOG_DEBUG("String is whitespace, returning an empty string");
        return "";
    }

    // Find the last non-whitespace character in the string
    const auto end_pos = str.find_last_not_of(" \t\n\r\f\v");

    // Remove leading and trailing whitespace
    const std::string trimmed_str = str.substr(start_pos, end_pos - start_pos + 1);

    // Return the trimmed string (RVO)
    LOG_DEBUG("Trimmed string: '" + trimmed_str + '\'');
    return trimmed_str;
}

std::string utils::strings::to_lower(const std::string &str)
{
    LOG_DEBUG("Converting string '" + str + "' to lowercase");

    // Create copy of the input string (so we can use RVO when returning the result)
    std::string lower_str = str;

    // Convert the string to lowercase
    std::transform(lower_str.cbegin(), lower_str.cend(), lower_str.begin(),
                   // std::to_lower expects an int, so we cast the char to an unsigned char
                   [](unsigned char c) { return std::tolower(c); });

    // Return the lowercase string (RVO)
    LOG_DEBUG("Lowercased string: '" + lower_str + '\'');
    return lower_str;
}

std::string utils::strings::remove_comments(const std::string &str)
{
    LOG_DEBUG("Removing comments from string '" + str + '\'');

    // Create copy of the input string (so we can use RVO when returning the result)
    std::string no_comments = str;

    // Find index of "//" in the string, then remove everything from the index to the end of the string
    if (const auto index = str.find("//"); index != std::string::npos) {
        no_comments.erase(index);
    }

    // Return the string without comments (RVO)
    LOG_DEBUG("String without comments: '" + no_comments + '\'');
    return no_comments;
}

std::string utils::strings::create_cpp_reference_link(const std::string &name)
{
    LOG_DEBUG("Creating a cppreference.com link for function '" + name + '\'');

    // URL encoding map
    // This list is not exhaustive, and only includes the characters that are most likely to appear in function names
    // If the link is broken, it's not critical, as it's primarily used as a suggestion to add include headers for standard library functions
    static const std::unordered_map<char, std::string> url_encoding = {
        {' ', "%20"},
        {'!', "%21"},
        {'#', "%23"},
        {'$', "%24"},
        {'&', "%26"},
        {'\'', "%27"},
        {'(', "%28"},
        {')', "%29"},
        {'*', "%2A"},
        {'+', "%2B"},
        {',', "%2C"},
        {'/', "%2F"},
        {':', "%3A"},
        {';', "%3B"},
        {'=', "%3D"},
        {'?', "%3F"},
        {'@', "%40"},
        {'[', "%5B"},
        {']', "%5D"},
    };

    // Initialize the link with the base URL for the DuckDuckGo search
    std::ostringstream link;
    link << "https://duckduckgo.com/?sites=cppreference.com&q=";

    // Iterate over each character in the provided function name (e.g., "std::string")
    // This ensures that all special characters are properly encoded
    for (const auto &character : name) {

        // Find the character in the URL encoding map (e.g., ":")
        const auto encoded = url_encoding.find(character);

        if (encoded != url_encoding.end()) {
            // If found, add the URL-encoded version of the character (e.g., "%3A")
            link << encoded->second;
            LOG_DEBUG("Using encoded character '" + encoded->second + '\'');
        }
        else {
            // Otherwise, add the original character (e.g., ":")
            link << character;
            LOG_DEBUG("Using original character '" + std::string(1, character) + '\'');
        }
    }

    // Add the final part of the URL
    link << "&ia=web";

    // Return the link as a string
    const std::string result = link.str();
    LOG_DEBUG("Link: '" + result + '\'');
    return result;
}
