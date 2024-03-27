/**
 * @file log.hpp
 *
 * @brief Logging utilities shared across the application.
 */

#pragma once

#include <string>  // for std::string

namespace shared {
namespace log {
namespace impl {

/**
 * @brief Helper function to print a debug-level message to the standard output.
 *
 * This function is used by the LOG_DEBUG macro. It only prints the message if the global variable "verbose" from "globals.hpp" is set to "true".
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void debug(const std::string &caller, const std::string &message);

/**
 * @brief Helper function to print an info-level message to the standard output.
 *
 * This function is used by the LOG_INFO macro.
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void info(const std::string &caller, const std::string &message);

/**
 * @brief Helper function to print a warning-level message to the standard output.
 *
 * This function is used by the LOG_WARNING macro.
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void warning(const std::string &caller, const std::string &message);

/**
 * @brief Helper function to print an error-level message to the standard output.
 *
 * This function is used by the LOG_ERROR macro.
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void error(const std::string &caller, const std::string &message);

}  // namespace impl
}  // namespace log
}  // namespace shared

/**
 * @brief Macro to print a debug-level message to the standard output.
 *
 * Unlike the other logging functions, this macro only prints the message if the global variable "globals::verbose" is set to "true".
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_DEBUG(message) shared::log::impl::debug(__func__, message)

/**
 * @brief Macro to print an info-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_INFO(message) shared::log::impl::info(__func__, message)

/**
 * @brief Macro to print a warning-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_WARNING(message) shared::log::impl::warning(__func__, message)

/**
 * @brief Macro to print an error-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_ERROR(message) shared::log::impl::error(__func__, message)
