/**
 * @file log.hpp
 *
 * @brief Logging utilities.
 */

#pragma once

#include <string>  // for std::string

namespace log {
namespace impl {

/**
 * @brief Print a debug-level message to the standard output.
 *
 * Unlike the other logging functions, this function only prints the message if the global variable "verbose" is set to "true".
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void debug(const std::string &caller, const std::string &message);

/**
 * @brief Print an info-level message to the standard output.
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void info(const std::string &caller, const std::string &message);

/**
 * @brief Print a warning-level message to the standard output.
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void warning(const std::string &caller, const std::string &message);

/**
 * @brief Print an error-level message to the standard output.
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void error(const std::string &caller, const std::string &message);

}  // namespace impl
}  // namespace log

/**
 * @brief Macro to print a debug-level message to the standard output.
 *
 * Unlike the other logging functions, this macro only prints the message if the global variable "globals::verbose" is set to "true".
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_DEBUG(message) log::impl::debug(__func__, message)

/**
 * @brief Macro to print an info-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_INFO(message) log::impl::info(__func__, message)

/**
 * @brief Macro to print a warning-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_WARNING(message) log::impl::warning(__func__, message)

/**
 * @brief Macro to print an error-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_ERROR(message) log::impl::error(__func__, message)
