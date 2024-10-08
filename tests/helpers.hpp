/**
 * @file helpers.hpp
 *
 * @brief Helper functions for automated tests.
 */

#pragma once

#include <filesystem>  // for std::filesystem
#include <vector>      // for std::vector

#include <fmt/core.h>
#include <fmt/ranges.h>

#include "modules/analyze.hpp"

namespace helpers {

/**
 * @brief Class that represents a temporary directory as a RAII object.
 *
 * On construction, the class creates a temporary directory on disk. When the object goes out of scope, the directory is removed recursively from disk.
 *
 * @note This class is marked as `final` to prevent inheritance.
 */
class TempDir final {
  public:
    /**
     * @brief Construct a new TempDir object.
     *
     * On construction, the temporary directory is removed recursively from disk, then created again to ensure it is empty.
     *
     * @param directory Path to the temporary directory (e.g., "~/data").
     */
    inline explicit TempDir(const std::filesystem::path &directory)
        : directory_(directory)
    {
        std::filesystem::remove_all(this->directory_);
        std::filesystem::create_directories(this->directory_);
    }

    /**
     * @brief Destroy the TempDir object.
     *
     * On destruction, the temporary directory is removed recursively from disk.
     */
    inline ~TempDir()
    {
        std::filesystem::remove_all(this->directory_);
    }

    /**
     * @brief Get the path to the temporary directory.
     *
     * @return Const reference to a path to the temporary directory provided in the constructor (e.g., "~/data").
     */
    [[nodiscard]] inline const std::filesystem::path &get() const
    {
        return this->directory_;
    }

  private:
    /**
     * @brief Path to the temporary directory provided in the constructor (e.g., "~/data").
     */
    const std::filesystem::path directory_;
};

/**
 * @brief Compare program-generated bare includes with expected bare includes.
 *
 * This also prints the results to the console.
 *
 * @param program Program-generated bare includes.
 * @param expected Expected bare includes.
 *
 * @return True if the program-generated bare includes match the expected bare includes, false otherwise.
 */
[[nodiscard]] inline bool compare_and_print_bare_includes(const std::vector<modules::analyze::BareInclude> &program,
                                                          const std::vector<modules::analyze::BareInclude> &expected)
{
    if (program != expected) {
        fmt::print(stderr, "Bare include test failed.\nExpected:\n");
        for (const auto &entry : expected) {
            fmt::print(stderr, "  Line '{}': '{}', Include: '{}'\n", entry.number, entry.text, entry.header);
        }
        fmt::print(stderr, "Actual:\n");
        for (const auto &entry : program) {
            fmt::print(stderr, "  Line '{}': '{}', Include: '{}'\n", entry.number, entry.text, entry.header);
        }
        return false;
    }

    fmt::print("Bare include test succeeded.\n");
    for (const auto &entry : program) {
        fmt::print("  Line '{}': '{}', Include: '{}'\n", entry.number, entry.text, entry.header);
    }
    return true;
}

/**
 * @brief Compare program-generated unused functions with expected unused functions.
 *
 * This also prints the results to the console.
 *
 * @param program Program-generated unused functions.
 * @param expected Expected unused functions.
 *
 * @return True if the program-generated unused functions match the expected unused functions, false otherwise.
 */
[[nodiscard]] inline bool compare_and_print_unused_functions(const std::vector<modules::analyze::IncludeWithUnusedFunctions> &program,
                                                             const std::vector<modules::analyze::IncludeWithUnusedFunctions> &expected)
{
    if (program != expected) {
        fmt::print(stderr, "Unused functions test failed.\nExpected:\n");
        for (const auto &entry : expected) {
            fmt::print(stderr, "  Line '{}': '{}', Unused Functions: '{}'\n", entry.number, entry.text, fmt::join(entry.unused_functions, ", "));
        }
        fmt::print(stderr, "Actual:\n");
        for (const auto &entry : program) {
            fmt::print(stderr, "  Line '{}': '{}', Unused Functions: '{}'\n", entry.number, entry.text, fmt::join(entry.unused_functions, ", "));
        }
        return false;
    }

    fmt::print("Unused functions test succeeded.\n");
    for (const auto &entry : program) {
        fmt::print("  Line '{}': '{}', Unused Functions: '{}'\n", entry.number, entry.text, fmt::join(entry.unused_functions, ", "));
    }
    return true;
}

/**
 * @brief Compare program-generated unlisted functions with expected unlisted functions.
 *
 * This also prints the results to the console.
 *
 * @param program Program-generated unlisted functions.
 * @param expected Expected unlisted functions.
 *
 * @return True if the program-generated unlisted functions match the expected unlisted functions, false otherwise.
 */
[[nodiscard]] inline bool compare_and_print_unlisted_functions(const std::vector<modules::analyze::UnlistedFunction> &program,
                                                               const std::vector<modules::analyze::UnlistedFunction> &expected)
{
    if (program != expected) {
        fmt::print(stderr, "Unlisted functions test failed.\nExpected:\n");
        for (const auto &entry : expected) {
            fmt::print(stderr, "  Line '{}': '{}', Function: '{}', Link: '{}'\n", entry.number, entry.text, entry.function, entry.link);
        }
        fmt::print(stderr, "Actual:\n");
        for (const auto &entry : program) {
            fmt::print(stderr, "  Line '{}': '{}', Function: '{}', Link: '{}'\n", entry.number, entry.text, entry.function, entry.link);
        }
        return false;
    }

    fmt::print("Unlisted functions test succeeded.\n");
    for (const auto &entry : program) {
        fmt::print("  Line '{}': '{}', Function: '{}', Link: '{}'\n", entry.number, entry.text, entry.function, entry.link);
    }
    return true;
}

}  // namespace helpers
