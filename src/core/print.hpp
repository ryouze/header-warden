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

/**
 * @brief Converts a vector to a string representation that looks similar to Python's representation.
 *
 * @tparam T Type of the elements in the vector, should be convertible to a string using `std::to_string` (e.g., "int").
 * @param vec Vector to convert to string (e.g., "{1, 2, 3}").
 *
 * @return String representation of the vector (e.g., "[1, 2, 3]").
 */
template <typename T>
std::string vector_to_string(const std::vector<T> &vec)
{
    std::ostringstream oss;
    oss << "[";
    for (const auto &item : vec) {
        oss << std::to_string(item) << ", ";
    }
    std::string str = oss.str();
    if (!vec.empty()) {
        str = str.substr(0, str.size() - 2);  // Remove the trailing comma and space
    }
    str += "]";
    return str;
}

}  // namespace print
}  // namespace core
