/**
 * @file print.hpp
 *
 * @brief Core printing functions for the application.
 */

#pragma once

#include <sstream>  // for std::ostringstream
#include <string>   // for std::string, std::to_string
#include <vector>   // for std::vector

namespace core {
namespace print {

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

    // Add the closing bracket and return the string stream as a string
    oss << "]";
    return oss.str();
}

}  // namespace print
}  // namespace core
