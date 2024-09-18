/**
 * @file io.hpp
 *
 * @brief Load lines of text from disk.
 */

#pragma once

#include <cstddef>    // for std::size_t
#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string
#include <vector>     // for std::vector

namespace core::io {

/**
 * @brief Base class for exceptions raised during I/O operations.
 */
class IOError : public std::runtime_error {
  public:
    explicit IOError(const std::string &message)
        : std::runtime_error(message) {}
};

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
 * @throws core::io::IOError If the file cannot be opened for reading or if any other I/O error occurs.
 */
[[nodiscard]] std::vector<Line> read_lines(const std::string &input_path,
                                           const std::size_t initial_capacity = 100);

}  // namespace core::io
