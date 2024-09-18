/**
 * @file main.cpp
 */

#include <cstdlib>    // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // for std::exception

#include <fmt/core.h>

#if defined(_WIN32)
#include <pathmaster/pathmaster.hpp>
#endif

#include "app.hpp"
#include "core/args.hpp"

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

#if defined(_WIN32)
        // Enable UTF-8 output on Windows
        pathmaster::setup_utf8_console_output();
#endif

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
