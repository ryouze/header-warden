#include "log.hpp"
#include "globals.hpp"

#include <chrono>    // for std::chrono
#include <ctime>     // for std::tm, localtime_r, localtime_s
#include <iomanip>   // for std::put_time
#include <iostream>  // for std::cout, std::cerr
#include <mutex>     // for std::mutex, std::lock_guard
#include <ostream>   // for std::ostream
#include <sstream>   // for std::ostringstream
#include <string>    // for std::string

namespace {

/**
 * @brief Private helper function to convert a time point to a formatted timestamp string.
 *
 * This is thread-safe.
 *
 * @param tp Time point to convert.
 * @return String representing the time point in the "YYYY-MM-DD HH:MM:SS" format.
 */
std::string compute_timestamp(const std::chrono::system_clock::time_point &tp)
{
    // Convert the time point to a time_t structure
    const auto tp_c = std::chrono::system_clock::to_time_t(tp);

    // Convert the time_t structure to a tm structure
    // This uses the appropriate function based on the operating system, because std::localtime is not thread-safe
    std::tm time_info;
#ifdef _WIN32  // Windows (32-bit and 64-bit)
    localtime_s(&time_info, &tp_c);
#else  // POSIX
    localtime_r(&tp_c, &time_info);
#endif

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
 * This is thread-safe.
 *
 * @return String representing the current time in the "YYYY-MM-DD HH:MM:SS" format (e.g., "2024-01-01 12:34:56").
 */
std::string get_current_time()
{
    // Lock the function to prevent multiple threads from computing the timestamp at the same time
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

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
 * This is thread-safe.
 *
 * @param os Output stream to print the message to (e.g., "std::cout", "std::cerr").
 * @param level Log level (e.g., "DEBUG", "INFO", "WARNING", "ERROR").
 * @param caller Name of the function that is logging the message (e.g., "main").
 * @param message Message to log (e.g., "hello").
 */
void print_formatted(std::ostream &os, const std::string &level, const std::string &caller, const std::string &message)
{
    // Lock the output stream to prevent interleaved output
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    // Print the formatted log message using the specified output stream (e.g., "std::cout", "std::cerr")
    os << get_current_time() << " | " << level << " | " << caller << " - " << message << '\n';
}

}  // namespace

void shared::log::impl::debug(const std::string &caller, const std::string &message)
{
    if (globals::verbose) {
        print_formatted(std::cout, "DEBUG  ", caller, message);
    }
}

void shared::log::impl::info(const std::string &caller, const std::string &message)
{
    print_formatted(std::cout, "INFO   ", caller, message);
}

void shared::log::impl::warning(const std::string &caller, const std::string &message)
{
    print_formatted(std::cerr, "WARNING", caller, message);
}

void shared::log::impl::error(const std::string &caller, const std::string &message)
{
    print_formatted(std::cerr, "ERROR  ", caller, message);
}
