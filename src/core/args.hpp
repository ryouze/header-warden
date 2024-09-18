/**
 * @file args.hpp
 *
 * @brief Process command-line arguments.
 */

#pragma once

#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string
#include <vector>     // for std::vector

namespace core::args {

/**
 * @brief Exceptions raised by command-line argument parser when help or version is requested. The requested message is returned.
 */
class ArgsError : public std::runtime_error {
  public:
    explicit ArgsError(const std::string &message)
        : std::runtime_error(message) {}
};

/**
 * @brief Struct that represents a set of enabled features.
 *
 * @note This struct is marked as `final` to prevent inheritance.
 */
struct Enable final {
    /**
     * @brief If true, enable bare include directives.
     */
    bool bare;

    /**
     * @brief If true, enable unused functions.
     */
    bool unused;

    /**
     * @brief If true, enable unlisted functions.
     */
    bool unlisted;
};

/**
 * @brief Class that represents command-line arguments.
 *
 * On construction, the class parses the command-line arguments, then sets the filepaths and enabled features.
 *
 * @note This class is marked as `final` to prevent inheritance.
 */
class Args final {
  public:
    /**
     * @brief Construct a new Args object.
     *
     * @param argc Number of command-line arguments (e.g., "2").
     * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
     *
     @throws ArgsError If failed to process command-line arguments.

     @note When help or version is requested, the class prints the requested message and exits immediately.
     */
    explicit Args(const int argc,
                  char **argv);

    /**
     * @brief Vector of file paths.
     */
    std::vector<std::string> filepaths;

    /**
     * @brief Struct of enabled features (e.g., "Enable(false, true, true)").
     */
    Enable enable;
};

}  // namespace core::args