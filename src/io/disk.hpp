/**
 * @file disk.hpp
 *
 * @brief Disk I/O functions.
 */

#pragma once

#include "../core/log.hpp"

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
     * @param number Line number (e.g., "5").
     * @param text Line text (e.g., "std::string foo()").
     */
    explicit Line(const std::size_t number,
                  const std::string &text)
        : number(number), text(text) {}

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
     * @param line Line where the include directive is found.
     * @param name Name of the included file (e.g., "#include <iostream>").
     */
    BareInclude(const Line &line,
                const std::string &name)
        : line(line), name(name) {}

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
     * @param line Line where the include directive is found.
     * @param name Name of the included file (e.g., "#include <iostream>").
     * @param functions Functions associated with the include directive (e.g., "std::cout").
     */
    IncludeWithFunctions(const Line &line,
                         const std::string &name,
                         const std::vector<std::string> &functions)
        : line(line), name(name), functions(functions) {}

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
     * @param line Line where the function is found.
     * @param functions Functions found on the line (e.g., "std::cout").
     */
    Functions(const Line &line,
              const std::vector<std::string> &functions)
        : line(line), functions(functions) {}

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
    [[nodiscard]] const std::vector<disk::BareInclude> &get_bare_includes() const;

    /**
     * @brief Get the unused functions.
     *
     * @return Vector of include directives whose associated functions are unused in the code (e.g., "#include <iostream> // for std::cout, std::cerr").
     */
    [[nodiscard]] const std::vector<disk::IncludeWithFunctions> &get_unused_functions() const;

    /**
     * @brief Get the missing functions.
     *
     * @return Vector of functions that are used in the code, but not listed as a comment in any include directive (e.g., "std::cout").
     */
    [[nodiscard]] const std::vector<disk::Functions> &get_missing_functions() const;

  private:
    /**
     * @brief Vector of bare include directives (e.g., "#include <iostream>").
     */
    std::vector<disk::BareInclude> bare_includes_;

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
