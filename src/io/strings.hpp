/**
 * @file strings.hpp
 *
 * @brief String manipulation functions.
 */

#pragma once

#include "core/log.hpp"

#include <ios>      // for std::ios_base
#include <sstream>  // for std::ostringstream
#include <string>   // for std::string, std::to_string
#include <vector>   // for std::vector

namespace io {
namespace strings {

namespace impl {

/**
 * @brief Convert an item to a string representation.
 *
 * This uses `std::to_string` to convert the item to a string.
 *
 * @tparam T Type of the item, convertible to a string (e.g., "int").
 * @param item Item to convert to a string (e.g., "5").
 *
 * @return String representation of the item (e.g., "5").
 *
 * @note The type T must have a valid overload for `std::to_string`.
 */
template <typename T>
[[nodiscard]] inline std::string format(const T &item)
{
    return std::to_string(item);
}

/**
 * @brief Convert a string to a quoted string representation.
 *
 * This simply adds double quotes around the string.
 *
 * @param item String to convert to a quoted string (e.g., "hello").
 *
 * @return Quoted string representation of the string (e.g., "\"hello\"").
 */
[[nodiscard]] inline std::string format(const std::string &item)
{
    return "\"" + item + "\"";
}

}  // namespace impl

/**
 * @brief Convert a vector to a string representation that looks similar to Python's representation.
 *
 * @tparam T Type of the elements in the vector (e.g., "int", "std::string").
 * @param vec Vector to convert to string (e.g., "{1, 2, 3}").
 *
 * @return String representation of the vector (e.g., "[1, 2, 3]").
 */
template <typename T>
[[nodiscard]] std::string vector_to_string(const std::vector<T> &vec)
{
    LOG_DEBUG("Converting vector of '" + std::to_string(vec.size()) + "' items to string");

    // Create a string stream and add the opening bracket
    std::ostringstream oss;
    oss << "[";

    // Add each item with a comma and space
    for (const auto &item : vec) {
        oss << impl::format(item) << ", ";
    }

    // If not empty, move back 2 positions in the stream (equivalent to removing the trailing comma and space)
    if (!vec.empty()) {
        oss.seekp(-2, std::ios_base::end);
    }

    // Add the closing bracket and turn the string stream into a string
    oss << "]";
    const std::string result = oss.str();

    // Return the string representation of the vector
    LOG_DEBUG("Vector to string: '" + result + '\'');
    return result;
}

/**
 * @brief Remove leading and trailing whitespace from string.
 *
 * @param str String to trim (e.g., "  hello  ").
 *
 * @return Trimmed string (e.g., "hello").
 */
[[nodiscard]] std::string trim_whitespace(const std::string &str);

/**
 * @brief Convert a string to lowercase.
 *
 * @param str String to convert to lowercase (e.g., "Hello World").
 *
 * @return Lowercase string (e.g., "hello world").
 */
[[nodiscard]] std::string to_lower(const std::string &str);

/**
 * @brief Remove comments from string.
 *
 * @param str String from which to remove comments (e.g., "int x = 5 // My comment").
 *
 * @return String without comments (e.g., "int x = 5").
 */
[[nodiscard]] std::string remove_comments(const std::string &str);

/**
 * @brief Create a URL that links to a DuckDuckGo search for the name provided on cppreference.com.
 *
 * @param name Name of the C++ function (or any other keyword) to create a link for (e.g., "algorithm", "std::string").
 *
 * @return String containing a URL.
 */
[[nodiscard]] std::string create_cpp_reference_link(const std::string &name);

}  // namespace strings
}  // namespace io
