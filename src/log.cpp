#include "log.hpp"
#include "globals.hpp"

#include <iostream>

#include <chrono>
#include <iomanip>
#include <sstream>

namespace {

/**
 * @brief Private helper function to get the current time.
 *
 * @return String representing the current time in the "YYYY-MM-DD HH:MM:SS.sss" format (e.g., "2024-01-01 12:34:56.789").
 */
std::string get_current_time()
{
    // Get the current time and convert to a time_t object
    const auto now = std::chrono::system_clock::now();
    const auto now_c = std::chrono::system_clock::to_time_t(now);

    // Get the milliseconds part of the time
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // Create a string stream and format the time as "YYYY-MM-DD HH:MM:SS.sss"
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << now_ms.count();

    // Return the formatted time
    return oss.str();
}

/**
 * @brief Private helper function to print a log message.
 *
 * @param os Output stream to print the message to (e.g., "std::cout", "std::cerr").
 * @param level Log level (e.g., "DEBUG", "INFO", "WARNING", "ERROR").
 * @param caller Name of the function that is logging the message (e.g., "main").
 * @param message Message to log (e.g., "hello").
 */
void print_formatted(std::ostream &os, const std::string &level, const std::string &caller, const std::string &message)
{
    os << get_current_time() << " | " << level << " | " << caller << " - " << message << '\n';
}

}  // namespace

void log::impl::debug_impl(const std::string &caller, const std::string &message)
{
    if (globals::verbose) {
        print_formatted(std::cout, "DEBUG  ", caller, message);
    }
}

void log::impl::info_impl(const std::string &caller, const std::string &message)
{
    print_formatted(std::cout, "INFO   ", caller, message);
}

void log::impl::warning_impl(const std::string &caller, const std::string &message)
{
    print_formatted(std::cerr, "WARNING", caller, message);
}

void log::impl::error_impl(const std::string &caller, const std::string &message)
{
    print_formatted(std::cerr, "ERROR  ", caller, message);
}
