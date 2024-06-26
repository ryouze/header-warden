/**
 * @file log.hpp
 *
 * @brief Core logging functions.
 */

#pragma once

#include <string>  // for std::string

namespace utils {
namespace log {

namespace impl {

/**
 * @brief Enum class representing the log level of a message.
 */
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
};

/**
 * @brief Helper function to print a log message to the standard output.
 *
 * This function is used by the logging macros.
 *
 * The debug-level messages are only printed if the global variable "globals::verbose" is set to "true".
 *
 * @param level Log level of the message (e.g., debug, info, warning, error).
 * @param caller Name of the calling function (e.g., "main").
 * @param line Line number in the source code file (e.g., "42").
 * @param message Message to print (e.g., "hello").
 */
void log(const LogLevel level,
         const std::string &caller,
         const long line,
         const std::string &message);

}  // namespace impl
}  // namespace log
}  // namespace utils

/**
 * @brief Macro to print a debug-level message to the standard output.
 *
 * Unlike the other logging functions, this macro only prints the message if the global variable "globals::verbose" is set to "true".
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_DEBUG(message) utils::log::impl::log(utils::log::impl::LogLevel::DEBUG, __func__, __LINE__, message)

/**
 * @brief Macro to print an info-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_INFO(message) utils::log::impl::log(utils::log::impl::LogLevel::INFO, __func__, __LINE__, message)

/**
 * @brief Macro to print a warning-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_WARNING(message) utils::log::impl::log(utils::log::impl::LogLevel::WARNING, __func__, __LINE__, message)

/**
 * @brief Macro to print an error-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_ERROR(message) utils::log::impl::log(utils::log::impl::LogLevel::ERROR, __func__, __LINE__, message)
