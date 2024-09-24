/**
 * @file string.hpp
 *
 * @brief Manipulate strings of text.
 */

#pragma once

#include <filesystem>  // for std::filesystem
#include <string>      // for std::string
#include <vector>      // for std::vector

namespace core::string {

/**
 * @brief Convert a vector of filesystem paths to a vector of strings.
 *
 * @param paths Vector of filesystem paths (e.g., {"/path/to/file1", "/path/to/file2"}).
 * @return Vector of strings (e.g., {"/path/to/file1", "/path/to/file2"}).
 */
[[nodiscard]] std::vector<std::string> paths_to_strings(const std::vector<std::filesystem::path> &v);

/**
 * @brief Convert a string to lowercase.
 *
 * @param str String to convert to lowercase (e.g., "Hello WORLD").
 *
 * @return Lowercase string (e.g., "hello world").
 */
[[nodiscard]] std::string to_lower(std::string str);

/**
 * @brief Strip leading and trailing whitespace from string.
 *
 * @param str String to strip whitespace from (e.g., "  Hello world!  ").
 *
 * @return String with whitespace stripped (e.g., "Hello world!").
 */
[[nodiscard]] std::string strip_whitespace(const std::string &str);

/**
 * @brief Remove a comment from string.
 *
 * This function removes everything from the first occurrence of "//" to the end of the string.
 *
 * @param str String from which the comment will be removed (e.g., "int x = 5 // My comment").
 *
 * @return String without the comment (e.g., "int x = 5").
 */
[[nodiscard]] std::string remove_comment(std::string str);

/**
 * @brief Create a URL that links to a DuckDuckGo search for the name provided, with websites limited to cppreference.com only.
 *
 * @param name Name of the function to search for (e.g., "std::string").
 *
 * @return String containing the URL (e.g., "https://duckduckgo.com/?sites=cppreference.com&q=std%3A%3Asort&ia=web").
 */
[[nodiscard]] std::string create_cpp_reference_link(const std::string &name);

}  // namespace core::string
