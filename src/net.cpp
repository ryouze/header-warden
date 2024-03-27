/**
 * @file net.cpp
 */

#include "net.hpp"
#include "shared/log.hpp"

#include <sstream>        // for std::ostringstream
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map

std::string net::create_cpp_reference_link(const std::string &name)
{
    LOG_DEBUG("Creating a link to cppreference.com for the function '" + name + '\'');

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
            LOG_DEBUG("Using encoded character: " + encoded->second);
        }
        else {
            // Otherwise, add the original character (e.g., ":")
            link << character;
            LOG_DEBUG("Using original character: " + std::string(1, character));
        }
    }

    // Add the final part of the URL
    link << "&ia=web";

    // Return the link as a string
    const std::string result = link.str();
    LOG_DEBUG("Created link: " + result);
    return result;
}
