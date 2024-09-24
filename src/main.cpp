/**
 * @file main.cpp
 */

#include <cstdlib>    // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // for std::exception

#include <fmt/core.h>

#include "app.hpp"
#include "core/args.hpp"
#include "core/io.hpp"

/**
 * @brief Entry-point of the application.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 *
 * @return EXIT_SUCCESS if the application ran successfully, EXIT_FAILURE otherwise.
 */
int main(int argc,
         char **argv)
{
    try {
        // Setup UTF-8 input/output on Windows
        core::io::setup_utf8_console();

        // Run the application
        app::run(argc, argv);
    }
    catch (const core::args::ArgsError &e) {
        // Failed to parse command-line arguments
        fmt::print("{}\n", e.what());
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }
    catch (...) {
        fmt::print(stderr, "Error: Unknown\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
