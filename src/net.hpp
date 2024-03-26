#pragma once

#include <string>  // for std::string

namespace net {

/**
 * @brief Create a URL that links to a DuckDuckGo search for the name provided on cppreference.com.
 *
 * @param name Name of the C++ function (or any other keyword) to create a link for (e.g., "algorithm", "std::string").
 *
 * @return String containing a URL.
 */
[[nodiscard]] std::string create_cpp_reference_link(const std::string &name);

}  // namespace net
