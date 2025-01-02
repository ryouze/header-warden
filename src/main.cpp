/**
 * @file main.cpp
 */

#include <cstdlib>    // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // for std::exception

#include <fmt/core.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>         // for SetConsoleCP, SetConsoleOutputCP, CP_UTF8
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
#if defined(_WIN32)  // Setup UTF-8 input/output
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    try {
        // Pass parsed command-line arguments to the application
        app::run(core::args::Args(argc, argv));
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
