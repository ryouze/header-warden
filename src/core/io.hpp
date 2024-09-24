/**
 * @file io.hpp
 *
 * @brief Input/output functions.
 */

#pragma once

#include <cstddef>     // for std::size_t
#include <filesystem>  // for std::filesystem
#include <stdexcept>   // for std::runtime_error
#include <string>      // for std::string
#include <vector>      // for std::vector

namespace core::io {

/**
 * @brief Setup UTF-8 input/output on Windows. Do nothing on other platforms.
 *
 * @throws std::runtime_error If failed to enable UTF-8 encoding on Windows.
 */
void setup_utf8_console();

/**
 * @brief Struct that represents a single line of text.
 */
struct Line {
    /**
     * @brief Construct a new Line object.
     *
     * @param _number Line number (e.g., "1").
     * @param _text Line text (e.g., "Hello world!").
     */
    explicit Line(const std::size_t _number,
                  const std::string &_text)
        : number(_number),
          text(_text) {}

    /**
     * @brief Line number (e.g., "1").
     */
    const std::size_t number;

    /**
     * @brief Line text (e.g., "Hello world!").
     */
    const std::string text;
};

/**
 * @brief Load a vector of Line structs from a text file on disk.
 *
 * The vector is indexed by line number, starting at 1.
 *
 * @param input_path Path to the text file (e.g., "~/data.txt").
 * @param initial_capacity Predicted number of lines in the file (default: 100). If more lines are found, the vector will resize automatically, this is merely a hint.
 *
 * @return Vector of Line structs (e.g., {Line(1, "Hello world!"), Line(2, "How are you?")}).
 *
 * @throws std::runtime_error If the file cannot be opened for reading or if any other I/O error occurs.
 */
[[nodiscard]] std::vector<Line> read_lines(const std::filesystem::path &input_path,
                                           const std::size_t initial_capacity = 100);

}  // namespace core::io
