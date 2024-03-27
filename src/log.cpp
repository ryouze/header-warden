#include "log.hpp"
#include "globals.hpp"

#include <iostream>

#include <chrono>
#include <iomanip>
#include <sstream>

namespace {

/**
 * @brief Private helper function to convert a time point to a formatted timestamp string.
 *
 * @param tp Time point to convert.
 * @return String representing the time point in the "YYYY-MM-DD HH:MM:SS" format.
 */
std::string compute_timestamp(const std::chrono::system_clock::time_point &tp)
{
    // Convert the time point to a time_t structure
    auto tp_c = std::chrono::system_clock::to_time_t(tp);

    // Convert the time_t structure to a tm structure
    std::tm *time_info = std::localtime(&tp_c);

    // Create an output string stream
    std::ostringstream oss;

    // Format the tm structure into the string stream in the "YYYY-MM-DD HH:MM:SS" format
    oss << std::put_time(time_info, "%Y-%m-%d %H:%M:%S");

    // Return the formatted timestamp string
    return oss.str();
}

/**
 * @brief Private helper function to get the current time.
 *
 * This computes the current time only once per second to improve performance.
 *
 * @return String representing the current time in the "YYYY-MM-DD HH:MM:SS" format (e.g., "2024-01-01 12:34:56").
 */
std::string get_current_time()
{
    // Get the current time
    auto now = std::chrono::system_clock::now();

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
 * @param message Message to log (e.g., "hello").
 */
void print_formatted(std::ostream &os, const std::string &level, const std::string &caller, const std::string &message)
{
    os << get_current_time() << " | " << level << " | " << caller << " - " << message << '\n';
}

}  // namespace

void log::impl::debug(const std::string &caller, const std::string &message)
{
    if (globals::verbose) {
        print_formatted(std::cout, "DEBUG  ", caller, message);
    }
}

void log::impl::info(const std::string &caller, const std::string &message)
{
    print_formatted(std::cout, "INFO   ", caller, message);
}

void log::impl::warning(const std::string &caller, const std::string &message)
{
    print_formatted(std::cerr, "WARNING", caller, message);
}

void log::impl::error(const std::string &caller, const std::string &message)
{
    print_formatted(std::cerr, "ERROR  ", caller, message);
}
