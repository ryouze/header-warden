/**
 * @file args.hpp
 *
 * @brief Core command-line argument parser.
 */

#pragma once

#include <string>  // for std::string
#include <vector>  // for std::vector

namespace core {
namespace args {

/**
 * @brief Class that parses command-line arguments.
 *
 * On construction, the arguments are stored as a vector of strings. The class provides methods to check for the existence of arguments and retrieve their values.
 *
 * @note The program's name (`argv[0]`) is not included in the list of arguments. It is stored separately as `program_name_`. Use `get_help()` to get a formatted help message with the program's name included.
 */
class ArgParser {
  public:
    /**
     * @brief Construct a new ArgParser object.
     *
     * @param argc Number of command-line arguments (e.g., "2").
     * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
     */
    explicit ArgParser(const int argc,
                       char **argv);

    /**
     * @brief Check if the list of arguments is empty.
     *
     * @return True if the list of arguments is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const;

    /**
     * @brief Check if the list contains a specific argument.
     *
     * @param arg Argument to search for (e.g., "-h").
     *
     * @return True if the list contains the argument, false otherwise.
     */
    [[nodiscard]] bool contains(const std::string &arg) const;

    /**
     * @brief Check if the list contains a specific argument.
     *
     * @param short_arg First argument to search for (e.g., "-h").
     * @param long_arg Second argument to search for (e.g., "--help").
     *
     * @return True if the list contains any of the two arguments, false otherwise.
     */
    [[nodiscard]] bool contains(const std::string &short_arg,
                                const std::string &long_arg) const;

    // /**
    //  * @brief Get the value of a keyword argument.
    //  *
    //  * @param arg Keyword argument to search for (e.g., "--file").
    //  *
    //  * @return Value of the argument (e.g., "data.txt").
    //  *
    //  * @throws std::runtime_error If the argument does not exist.
    //  */
    // [[nodiscard]] const std::string &value(const std::string &arg) const;

    // /**
    //  * @brief Get the value of a keyword argument.
    //  *
    //  * @param short_arg First keyword argument to search for (e.g., "-f").
    //  * @param long_arg Second keyword argument to search for (e.g., "--file").
    //  *
    //  * @return Value of the argument (e.g., "data.txt").
    //  *
    //  * @throws std::runtime_error If the argument does not exist.
    //  */
    // [[nodiscard]] const std::string &value(const std::string &short_arg,
    //                                        const std::string &long_arg) const;

    // /**
    //  * @brief Get the value of a positional argument.
    //  *
    //  * @param index Position of the argument (e.g., "0"). The list of arguments does not include the program's name, so "0" refers to the first argument after the program's name.
    //  *
    //  * @return Value of the argument (e.g., "data.txt").
    //  *
    //  * @throws std::out_of_range If the index is out of range.
    //  */
    // [[nodiscard]] const std::string &position(const std::size_t index) const;

    // /**
    //  * @brief Get all arguments, both positional and keyword.
    //  *
    //  * @return All arguments as strings (e.g., {"data/file1.txt", "data/file2.txt", "--verbose", "-h"}).
    //  */
    // [[nodiscard]] const std::vector<std::string> &get_arguments() const;

    /**
     * @brief Get all positional arguments.
     *
     * Positional arguments are arguments that do not start with "-" (e.g., "-v") or "--" (e.g., "--verbose").
     *
     * @return All positional arguments (e.g., {"data/file1.txt", "data/file2.txt"}).
     */
    [[nodiscard]] std::vector<std::string> get_positional_arguments() const;

    /**
     * @brief Get formatted help message for the program.
     *
     * @return Formatted help message (e.g., "Usage: ./bin").
     */
    [[nodiscard]] std::string get_help() const;

  private:
    /**
     * @brief Name of the program (e.g., "./bin").
     */
    std::string program_name_;

    /**
     * @brief All arguments, both positional and keyword (e.g., {"data/file1.txt", "data/file2.txt", "--verbose", "-h"}).
     */
    std::vector<std::string> args_;
};

}  // namespace args
}  // namespace core
