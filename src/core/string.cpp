/**
 * @file string.cpp
 */

#include <algorithm>      // for std::transform, std::find_if_not
#include <cctype>         // for std::tolower, std::isspace
#include <cstddef>        // for std::size_t
#include <sstream>        // for std::ostringstream
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map

#include "string.hpp"

std::string core::string::to_lower(std::string str)
{
    std::transform(str.cbegin(), str.cend(), str.begin(),
                   // Since std::to_lower expects an "int", we cast "char" to "unsigned char"
                   [](const unsigned char c) { return std::tolower(c); });
    return str;
}

std::string core::string::strip_whitespace(const std::string &str)
{
    // Find the first non-whitespace character from the start
    const auto start = std::find_if_not(str.cbegin(), str.cend(),
                                        [](const unsigned char ch) { return std::isspace(ch); });

    // Find the first non-whitespace character from the end
    const auto end = std::find_if_not(str.crbegin(), str.crend(),
                                      [](const unsigned char ch) { return std::isspace(ch); })
                         .base();  // Convert reverse iterator to normal iterator

    // Return a substring starting from 'start' to 'end'
    return (start < end) ? std::string(start, end) : "";
}

std::string core::string::remove_comment(std::string str)
{
    // Find index of "//" in the string, then remove everything from the index to the end of the string
    if (const std::size_t index = str.find("//"); index != std::string::npos) {
        str.erase(index);
    }
    return str;
}

std::string core::string::create_cpp_reference_link(const std::string &name)
{
    // URL encoding map
    // This list is not exhaustive and only includes characters most likely to appear in function names
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

    // Iterate over each character in the provided function name (e.g., "std::string") to encode it
    for (const char character : name) {

        // Find the character in the URL encoding map (e.g., ":")
        if (const auto &encoded = url_encoding.find(character); encoded != url_encoding.cend()) {
            // If found, add the URL-encoded version of the character (e.g., "%3A")
            link << encoded->second;
        }
        else {
            // Otherwise, add the original character (e.g., ":")
            link << character;
        }
    }

    // Add the final part of the URL
    link << "&ia=web";

    // Return the link as a string
    return link.str();
}
