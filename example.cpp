/**
 * @file example.hpp
 *
 * @brief Example of a badly formatted file.
 */

// Bare include: This include does not have any accompanying comments
#include <iostream>

// Unused functions listed as comments: This include has a function (std::find) that is not actually used within the file
#include <algorithm>  // for std::find

// OK: This include is correctly documented with the used functions listed in the comments
#include <string>  // for std::string, std::to_string
#include <vector>  // for std::vector

// OK: This function uses "std::string" and "std::to_string" from the <string> header and "std::vector" from the <vector> header
std::vector<std::string> foo(const std::vector<int> &vec)
{
    std::vector<std::string> result;
    for (const auto &i : vec) {
        result.emplace_back(std::to_string(i));
    }
    return result;
}

// Unlisted function: This function uses "std::sort", but it's not listed in the comments after the includes
std::vector<int> bar(const std::vector<int> &v)
{
    std::vector<int> result(v);
    std::sort(result.begin(), result.end());
    return result;
}
