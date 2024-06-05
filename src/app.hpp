/**
 * @file app.hpp
 *
 * @brief Main application logic.
 */

#pragma once

#include <string>  // for std::string
#include <vector>  // for std::vector

namespace app {

/**
 * @brief Process all files and log reports to the console.
 *
 * Each file is processed separately. Any exceptions will be caught and logged.
 *
 * @param file_paths Vector of file paths to process (e.g., {"src/app.hpp", "src/main.cpp"}).
 */
void run(const std::vector<std::string> &file_paths);

}  // namespace app
