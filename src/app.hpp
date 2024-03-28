/**
 * @file app.hpp
 *
 * @brief Main application logic.
 */

#pragma once

#include <string>  // for std::string

namespace app {

/**
 * @brief Get report for a file.
 *
 * @param file_path Path to the file to process (e.g., "src/app.hpp").
 * @return Report for the file. This is a formatted string that lists any issues found in the file.
 */
[[nodiscard]] std::string get_report(const std::string &file_path);

}  // namespace app
