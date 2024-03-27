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
 * This computes the current time only once per second to improve performance.
 *
 * @return String representing the current time in the "YYYY-MM-DD HH:MM:SS" format (e.g., "2024-01-01 12:34:56").
 */
std::string get_current_time()
{
    static auto now = std::chrono::system_clock::now();
    static auto last_time = now;

    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *time_info = std::localtime(&now_c);
    std::ostringstream oss;
    oss << std::put_time(time_info, "%Y-%m-%d %H:%M:%S");
    static std::string last_timestamp = oss.str();

    // Get the current time
    now = std::chrono::system_clock::now();

    // Check if the current time is still within the same second as the last computed timestamp
    if (now - last_time < std::chrono::seconds(1)) {
        // If it is, reuse the last computed timestamp
        // std::cout << "REUSED\n";
        return last_timestamp;
    }

    // If it's not, compute a new timestamp
    now_c = std::chrono::system_clock::to_time_t(now);

    time_info = std::localtime(&now_c);
    oss.str("");
    oss << std::put_time(time_info, "%Y-%m-%d %H:%M:%S");

    // Store the new timestamp and the time when it was computed
    last_timestamp = oss.str();
    last_time = now;

    // std::cout << "NEW\n";
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
