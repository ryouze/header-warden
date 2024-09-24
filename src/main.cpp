/**
 * @file main.cpp
 */

#include <cstdlib>    // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // for std::exception

#include <fmt/core.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <locale>            // for setlocale, LC_ALL
#include <windows.h>         // for WideCharToMultiByte, GetLastError, CP_UTF8, SetConsoleCP, SetConsoleOutputCP
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
        if (!SetConsoleCP(CP_UTF8) || !SetConsoleOutputCP(CP_UTF8)) {
            if (throw_on_error) {
                throw PathmasterError("Failed to set UTF-8 code page on Windows: " + std::to_string(GetLastError()));
            }
        }

        if (!setlocale(LC_ALL, ".UTF8")) {
            if (throw_on_error) {
                throw PathmasterError("Failed to set UTF-8 locale on Windows");
            }
        }
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
