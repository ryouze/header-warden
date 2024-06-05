/**
 * @file globals.hpp
 *
 * @brief Global variables.
 */

#pragma once

namespace core {
namespace globals {

/**
 * @brief True if verbose output is enabled, false otherwise.
 */
inline bool verbose = false;

/**
 * @brief True if bare includes should be printed, false otherwise.
 */
inline bool bare = true;

/**
 * @brief True if unused includes should be printed, false otherwise.
 */
inline bool unused = true;

/**
 * @brief True if unlisted includes should be printed, false otherwise.
 */
inline bool unlisted = true;

}  // namespace globals
}  // namespace core
