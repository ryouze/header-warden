#pragma once

#include <cstddef>  // for std::size_t
#include <regex>    // for std::regex
#include <string>   // for std::string
#include <vector>   // for std::vector

namespace disk {

/**
 * @brief Enum class representing the type of a line in a file.
 */
enum class LineType {
    BARE_INCLUDE,           // Represents a line with an include directive (e.g., "#include <string>")
    INCLUDE_WITH_FUNCTION,  // Represents a line with an include directive and standard library functions (e.g., "#include <iostream> // for std::cout, std::cerr")
    FUNCTION,               // Represents a line with a standard library function (e.g., "std::string")
    EMPTY,                  // Represents an empty line
};

/**
 * @brief Class that represents a single line of source code.
 *
 * On construction, the line's content is parsed and its type is determined. The include directive and standard library functions are extracted from the line's text and stored as the object's members.
 */
class Line {
  public:
    /**
     * @brief Construct a new Line object.
     *
     * @param line_number Line's number (e.g., 20).
     * @param line_text Line's text (e.g., "hello").
     */
    explicit Line(const std::size_t line_number,
                  const std::string &line_text);

    /**
     * @brief Destroy the Line object.
     */
    ~Line() = default;

    /**
     * @brief Get the line number.
     *
     * @return Line number (e.g., "20").
     */

    [[nodiscard]] std::size_t get_number() const;

    /**
     * @brief Get the line text.
     *
     * @return Line text (e.g., "hello").
     */
    [[nodiscard]] const std::string &get_text() const;

    /**
     * @brief Get the line type.
     *
     * This is based on the line's content, and can be one of the following:
     * - "BARE_INCLUDE": Represents a line with a bare include directive (e.g., "#include <string>").
     * - "INCLUDE_WITH_FUNCTION": Represents a line with an include directive and standard library functions (e.g., "#include <iostream> // for std::cout, std::cerr").
     * - "FUNCTION": Represents a line with a standard library function (e.g., "std::string").
     * - "EMPTY": Represents an empty line.
     *
     * @return Line type (e.g., "BARE_INCLUDE").
     */
    [[nodiscard]] LineType get_type() const;

    /**
     * @brief Get the standard library functions without the "std::" prefix.
     *
     * @return Standard library functions (e.g., {"cout", "cerr"}). If no functions are present, an empty vector is returned.
     */
    [[nodiscard]] const std::vector<std::string> &get_functions() const;

    /**
     * @brief Get the standard library without the "#include <>" directive.
     *
     * @return Library name (e.g., "iostream"). If no library is included in the line, an empty string is returned.
     */
    [[nodiscard]] const std::string &get_include() const;

  private:
    /**
     * @brief Line number (e.g., "20").
     */
    const std::size_t number_;

    /**
     * @brief Line text (e.g., "hello").
     */
    const std::string text_;

    /**
     * @brief Line type (e.g., "BARE_INCLUDE").
     */
    LineType type_;

    /**
     * @brief Standard library functions (e.g., {"cout", "cerr"}).
     */
    std::vector<std::string> functions_;

    /**
     * @brief Library name (e.g., "iostream").
     */
    std::string include_;

    /**
     * @brief Check if the line contains a given regular expression pattern.
     *
     * @param regex_pattern Regular expression pattern to search for (e.g., "he.*o").
     *
     * @return True if the line contains the regex pattern, false otherwise (e.g., "true").
     */
    [[nodiscard]] bool contains_regex(const std::regex &regex_pattern) const;

    /**
     * @brief Get the first match of a regular expression pattern in the line.
     *
     * @param regex_pattern Regular expression pattern to search for (e.g., "he(.*)o").
     * @param capture_group Capture group to be used. If set to "0", the entire matched string is returned. If set to "1", the first capture group is returned. Defaults to "0".
     *
     * @return First match for the given regex pattern and capture group (e.g., "ll"). If no match is found, an empty string is returned.
     */
    [[nodiscard]] std::string get_first_regex_match(const std::regex &regex_pattern,
                                                    const std::size_t capture_group = 0) const;

    /**
     * @brief Get all matches of a regular expression pattern in the line.
     *
     * @param regex_pattern Regular expression pattern to search for (e.g., "he(.*)o").
     * @param capture_group Capture group to be used. If set to "0", the entire matched string is returned. If set to "1", the first capture group is returned. Defaults to "0".
     *
     * @return All matches for the given regex pattern and capture group (e.g., {"ll", "a"}). If no matches are found, an empty vector is returned.
     */
    [[nodiscard]] std::vector<std::string> get_all_regex_matches(const std::regex &regex_pattern,
                                                                 const std::size_t capture_group = 0) const;
};

/**
 * @brief Class that represents a source code file.
 *
 * On construction, the file's content is read and stored as the object member.
 */
class File {
  public:
    /**
     * @brief Construct a new File object.
     *
     * @param file_pat Path to the source code file (e.g., "~/data/file.txt").
     * @param turn_lowercase Whether to turn the source code lowercase. If set to "true", the source code will be turned lowercase. If set to "false", the source code will be left in its original case, which might cause problems with regular expressions. Defaults to "true".
     *
     * @throws std::runtime_error if failed to open the source code file.
     */
    explicit File(const std::string &file_path,
                  const bool turn_lowercase = true);

    /**
     * @brief Destroy the File object.
     */
    ~File() = default;

    /**
     * @brief Get all matches of a regular expression pattern in the line.
     *
     * @param regex_pattern Regular expression pattern to search for (e.g., "he(.*)o").
     * @param capture_group Capture group to be used. If set to "0", the entire matched string is returned. If set to "1", the first capture group is returned. Defaults to "0".
     *
     * @return All matches for the given regex pattern and capture group (e.g., {"ll", "a"}). If no matches are found, an empty vector is returned.
     */

    /**
     * @brief Get all lines of the source code file.
     *
     * The Line object automatically parses the line's content and type on construction. It contains line number, text, type, and other members.
     *
     * @return All lines as Line objects.
     */
    [[nodiscard]] const std::vector<Line> &get_lines() const;

  private:
    /**
     * @brief Lines of the source code file.
     */
    std::vector<Line> lines_;
};

}  // namespace disk
