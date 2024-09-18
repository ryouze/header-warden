/**
 * @file app.hpp
 *
 * @brief Main application logic.
 */

#pragma once

namespace app {

/**
 * @brief Parse command-line arguments and run the application.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 */
void run(const int argc,
         char **argv);

}  // namespace app
