/**
 * @file log.cpp
 */

#include <chrono>    // for std::chrono
#include <ctime>     // for std::tm, std::localtime
#include <iomanip>   // for std::put_time
#include <iostream>  // for std::cout, std::cerr
#include <ostream>   // for std::ostream
#include <sstream>   // for std::ostringstream
#include <string>    // for std::string

#include "../core/globals.hpp"
#include "log.hpp"

namespace {

/**
 * @brief Private helper function to convert a time point to a formatted timestamp string.
 *
 * @param tp Time point to convert.
 * @return String representing the time point in the "YYYY-MM-DD HH:MM:SS" format.
 */
[[nodiscard]] std::string compute_timestamp(const std::chrono::system_clock::time_point &tp)
{
    // Convert the time point to a time_t structure
    const auto tp_c = std::chrono::system_clock::to_time_t(tp);

    // Convert the time_t structure to a tm structure
    const std::tm time_info = *std::localtime(&tp_c);

    // Create an output string stream and format the tm structure into it
    std::ostringstream oss;
    oss << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S");

    // Return the formatted timestamp string
    return oss.str();
}

/**
 * @brief Private helper function to get the current time.
 *
 * The time is cached and only updated once per second.
 *
 * @return String representing the current time in the "YYYY-MM-DD HH:MM:SS" format (e.g., "2024-01-01 12:34:56").
 */
[[nodiscard]] std::string get_current_time()
{
    // Get the current time
    const auto now = std::chrono::system_clock::now();

    // Static variables to store the last computed timestamp and the time when it was computed
    static auto last_time = now;
    static std::string last_timestamp = compute_timestamp(now);

    // Check if the current time is still within the same second as the last computed timestamp
    if (now - last_time >= std::chrono::seconds(1)) {
        // If it's not, compute a new timestamp
        last_timestamp = compute_timestamp(now);
        last_time = now;
    }

    // Return the last computed timestamp (either the current time or the last computed time)
    return last_timestamp;
}

/**
 * @brief Private helper function to print a log message.
 *
 * @param os Output stream to print the message to (e.g., "std::cout", "std::cerr").
 * @param level Log level (e.g., "DEBUG", "INFO", "WARNING", "ERROR").
 * @param caller Name of the function that is logging the message (e.g., "main").
 * @param line Line number in the source code file (e.g., "42").
 * @param message Message to log (e.g., "hello").
 */
void print_formatted(std::ostream &os,
                     const std::string &level,
                     const std::string &caller,
                     const long line,
                     const std::string &message)
{
    // Print the formatted log message using the specified output stream (e.g., "std::cout", "std::cerr")
    os << get_current_time() << " | " << level << " | " << caller << ":" << line << " - " << message << '\n';
}

}  // namespace

void utils::log::impl::log(const utils::log::impl::LogLevel level,
                           const std::string &caller,
                           const long line,
                           const std::string &message)
{
    switch (level) {
    case utils::log::impl::LogLevel::DEBUG:
        // Only print if verbose mode is enabled
        if (core::globals::verbose) {
            print_formatted(std::cout, "DEBUG  ", caller, line, message);
        }
        break;
    case utils::log::impl::LogLevel::INFO:
        print_formatted(std::cout, "INFO   ", caller, line, message);
        break;
    case utils::log::impl::LogLevel::WARNING:
        print_formatted(std::cerr, "WARNING", caller, line, message);
        break;
    case utils::log::impl::LogLevel::ERROR:
        print_formatted(std::cerr, "ERROR  ", caller, line, message);
        break;
    }
}
