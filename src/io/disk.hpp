/**
 * @file disk.hpp
 *
 * @brief Disk I/O functions.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
#include <vector>   // for std::vector

namespace io {
namespace disk {

/**
 * @brief Struct that represents a single line in a file.
 */
struct Line {

    /**
     * @brief Construct a new Line object.
     *
     * @param _number Line number (e.g., "5").
     * @param _text Line text (e.g., "std::string foo()").
     */
    explicit Line(const std::size_t _number,
                  const std::string &_text)
        : number(_number), text(_text) {}

    /**
     * @brief Line number (e.g., "5").
     */
    const std::size_t number;

    /**
     * @brief Line text (e.g., "std::string foo()").
     */
    const std::string text;
};

/**
 * @brief Struct that represents a bare include directive in a file.
 */
struct BareInclude {

    /**
     * @brief Construct a new BareInclude object.
     *
     * @param _line Line where the include directive is found.
     * @param _name Name of the included file (e.g., "#include <iostream>").
     */
    explicit BareInclude(const Line &_line,
                         const std::string &_name)
        : line(_line), name(_name) {}

    /**
     * @brief Line where the include directive is found.
     */
    const Line line;

    /**
     * @brief Name of the included file (e.g., "#include <iostream>").
     */
    const std::string name;
};

/**
 * @brief Struct that represents an include directive with associated functions in a file.
 */
struct IncludeWithFunctions {

    /**
     * @brief Construct a new IncludeWithFunctions object.
     *
     * @param _line Line where the include directive is found.
     * @param _name Name of the included file (e.g., "#include <iostream>").
     * @param _functions Functions associated with the include directive (e.g., "std::cout").
     */
    explicit IncludeWithFunctions(const Line &_line,
                                  const std::string &_name,
                                  const std::vector<std::string> &_functions)
        : line(_line), name(_name), functions(_functions) {}

    /**
     * @brief Line where the include directive is found.
     */
    const Line line;

    /**
     * @brief Name of the included file (e.g., "#include <iostream>").
     */
    const std::string name;

    /**
     * @brief Functions associated with the include directive (e.g., "std::cout").
     */
    const std::vector<std::string> functions;
};

/**
 * @brief Struct that represents a free-standing function not in an include directive in a file.
 */
struct Functions {

    /**
     * @brief Construct a new Functions object.
     *
     * @param _line Line where the function is found.
     * @param _functions Functions found on the line (e.g., "std::cout").
     */
    explicit Functions(const Line &_line,
                       const std::vector<std::string> &_functions)
        : line(_line), functions(_functions) {}

    /**
     * @brief Line where the function is found.
     */
    const Line line;

    /**
     * @brief Functions found on the line.
     */
    const std::vector<std::string> functions;
};

/**
 * @brief Class that extracts information from a file.
 *
 * On construction, the class extracts information from a file, such as include directives and functions.
 */
class File {
  public:
    /**
     * @brief Construct a new File object.
     *
     * @param file_path Path to the file to load (e.g., "src/main.cpp").
     */
    explicit File(const std::string &file_path);

    /**
     * @brief Get the bare includes.
     *
     * @return Vector of bare include directives (e.g., "#include <iostream>").
     */
    [[nodiscard]] const std::vector<BareInclude> &get_bare_includes() const;

    /**
     * @brief Get the unused functions.
     *
     * @return Vector of include directives whose associated functions are unused in the code (e.g., "#include <iostream> // for std::cout, std::cerr").
     */
    [[nodiscard]] const std::vector<IncludeWithFunctions> &get_unused_functions() const;

    /**
     * @brief Get the missing functions.
     *
     * @return Vector of functions that are used in the code, but not listed as a comment in any include directive (e.g., "std::cout").
     */
    [[nodiscard]] const std::vector<Functions> &get_missing_functions() const;

  private:
    /**
     * @brief Vector of bare include directives (e.g., "#include <iostream>").
     */
    std::vector<BareInclude> bare_includes_;

    /**
     * @brief Vector of include directives whose associated functions are unused in the code (e.g., "#include <iostream> // for std::cout, std::cerr").
     */
    std::vector<IncludeWithFunctions> unused_functions_;

    /**
     * @brief Vector of functions that are used in the code, but not listed as a comment in any include directive (e.g., "std::cout").
     */
    std::vector<Functions> missing_functions_;
};

}  // namespace disk
}  // namespace io
