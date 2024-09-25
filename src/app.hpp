/**
 * @file app.hpp
 *
 * @brief Main application logic.
 */

#pragma once

#include "core/args.hpp"

namespace app {

/**
 * @brief Run the application.
 *
 * @param args Parsed command-line arguments.
 */
void run(const core::args::Args &args);

}  // namespace app
