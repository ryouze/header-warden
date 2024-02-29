#include "net.hpp"

#include <string>  // for std::string

namespace net {

[[nodiscard]] std::string create_cpp_reference_link(const std::string &name)
{
    // Initialize the link with the base URL for the DuckDuckGo search
    std::string link = "https://duckduckgo.com/?sites=cppreference.com&q=";
    // Iterate over each character in the function name
    for (const auto &character : name) {
        // URL-encode special characters
        // This list is not exhaustive, and only includes the characters that are most likely to appear in function names
        // If the link is broken, it's not critical, as it's primarily used as a suggestion to add include headers for standard library functions
        switch (character) {
        case ' ':
            link += "%20";
            break;
        case '!':
            link += "%21";
            break;
        case '#':
            link += "%23";
            break;
        case '$':
            link += "%24";
            break;
        case '&':
            link += "%26";
            break;
        case '\'':
            link += "%27";
            break;
        case '(':
            link += "%28";
            break;
        case ')':
            link += "%29";
            break;
        case '*':
            link += "%2A";
            break;
        case '+':
            link += "%2B";
            break;
        case ',':
            link += "%2C";
            break;
        case '/':
            link += "%2F";
            break;
        case ':':
            link += "%3A";
            break;
        case ';':
            link += "%3B";
            break;
        case '=':
            link += "%3D";
            break;
        case '?':
            link += "%3F";
            break;
        case '@':
            link += "%40";
            break;
        case '[':
            link += "%5B";
            break;
        case ']':
            link += "%5D";
            break;
        default:
            // If the character is not a special character, add it to the link as is
            link += character;
            break;
        }
    }
    // Add the final part of the URL
    link += "&ia=web";
    return link;
}

}  // namespace net
