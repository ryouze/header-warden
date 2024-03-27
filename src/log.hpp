/**
 * @file log.hpp
 * @brief Logging utilities for the application.
 *
 * This header file provides a set of logging utilities for the application. It defines a set of macros (`LOG_DEBUG`, `LOG_INFO`, `LOG_WARNING`, and `LOG_ERROR`) that print messages to the standard output or standard error, depending on the log level. These macros automatically include the name of the calling function and the message to print.
 *
 * The actual printing is done by a set of functions (`debug_impl`, `info_impl`, `warning_impl`, and `error_impl`) in the `log::impl` namespace. These functions should not be called directly; instead, use the `LOG_DEBUG`, `LOG_INFO`, `LOG_WARNING`, and `LOG_ERROR` macros.
 *
 * The `LOG_DEBUG` macro only prints the message if the global variable `globals::verbose` is set to `true`.
 */
#pragma once

#include <string>

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
void debug_impl(const std::string &caller, const std::string &message);

/**
 * @brief Print an info-level message to the standard output.
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void info_impl(const std::string &caller, const std::string &message);

/**
 * @brief Print a warning-level message to the standard output.
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void warning_impl(const std::string &caller, const std::string &message);

/**
 * @brief Print an error-level message to the standard output.
 *
 * @param caller Name of the calling function (e.g., "main").
 * @param message Message to print (e.g., "hello").
 */
void error_impl(const std::string &caller, const std::string &message);

}  // namespace impl
}  // namespace log

/**
 * @brief Macro to print a debug-level message to the standard output.
 *
 * Unlike the other logging functions, this macro only prints the message if the global variable "globals::verbose" is set to "true".
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_DEBUG(message) log::debug_impl(__func__, message)

/**
 * @brief Macro to print an info-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_INFO(message) log::info_impl(__func__, message)

/**
 * @brief Macro to print a warning-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_WARNING(message) log::warning_impl(__func__, message)

/**
 * @brief Macro to print an error-level message to the standard output.
 *
 * @param message Message to print (e.g., "hello").
 */
#define LOG_ERROR(message) log::error_impl(__func__, message)
