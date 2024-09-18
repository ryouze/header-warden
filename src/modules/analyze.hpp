/**
 * @file analyze.hpp
 *
 * @brief Load C++ code from disk and analyze it.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
#include <vector>   // for std::vector

#include "core/io.hpp"

namespace modules::analyze {

/**
 * @brief Struct that represents a single bare include directive, i.e., without any standard functions listed after it as comments.
 *
 * E.g., "#include <iostream>" instead of "#include <iostream>  // for std::cout".
 *
 * @note This struct is marked as `final` to prevent inheritance. The "header" member variable stores the entire include directive including the "#include <>" part.
 */
struct BareInclude final : public core::io::Line {
    /**
     * @brief Construct a new BareInclude object.
     *
     * @param _number Original line number (e.g., "11").
     * @param _text Original line text where the include directive was found (e.g., "#include <iostream>").
     * @param _header Extracted include directive (e.g., "#include <iostream>").
     */
    explicit BareInclude(const std::size_t _number,
                         const std::string &_text,
                         const std::string &_header)
        : core::io::Line(_number, _text),
          header(_header) {}

    [[nodiscard]] bool operator==(const BareInclude &other) const
    {
        return number == other.number && text == other.text && header == other.header;
    }

    /**
     * @brief Extracted include directive, including the "#include <>" part (e.g., "#include <iostream>").
     */
    const std::string header;
};

/**
 * @brief Struct that represents a single include directive with a list of functions that are listed in comments but may not be used in the code.
 *
 * E.g., "#include <algorithm>  // for std::sort, std::find" where "std::find" is not used in the code.
 *
 * @note This struct is marked as `final` to prevent inheritance. All functions listed are stored with the "std::" prefix internally.
 */
struct IncludeWithUnusedFunctions final : public core::io::Line {
    /**
     * @brief Construct a new IncludeWithUnusedFunctions object.
     *
     * @param _number Original line number (e.g., "11").
     * @param _text Original line text where the include directive was found (e.g., "#include <iostream>  // for std::cout").
     * @param _unused_functions List of unused functions extracted from the comments, all prefixed with "std::" (e.g., {"std::find"}).
     */
    explicit IncludeWithUnusedFunctions(const std::size_t _number,
                                        const std::string &_text,
                                        const std::vector<std::string> &_unused_functions)
        : core::io::Line(_number, _text),
          unused_functions(_unused_functions) {}

    [[nodiscard]] bool operator==(const IncludeWithUnusedFunctions &other) const
    {
        return number == other.number && text == other.text && unused_functions == other.unused_functions;
    }

    /**
     * @brief List of functions listed as comments in the include directive, prefixed with "std::", which may be checked for actual usage.
     */
    const std::vector<std::string> unused_functions;
};

/**
 * @brief Struct that represents a single unlisted standard function, i.e., a function used in the code but not listed as a comment after an include directive.
 *
 * E.g., "std::sort()" is used in the code, but the include directive "#include <algorithm> // for std::find" is missing.
 *
 * @note This struct is marked as `final` to prevent inheritance. All standard functions are stored with the "std::" prefix internally.
 */
struct UnlistedFunction final : public core::io::Line {
    /**
     * @brief Construct a new UnlistedFunction object.
     *
     * @param _number Original line number (e.g., "31").
     * @param _text Original line text where the unlisted function was found (e.g., "std::sort(result.begin(), result.end());").
     * @param _function Unlisted function that needs to be added to include comments, prefixed with "std::" (e.g., "std::sort").
     * @param _link Link to the C++ reference for the function (e.g., "https://duckduckgo.com/?q=std%3A%3Asort+site%3Acppreference.com&ia=web").
     */
    explicit UnlistedFunction(const std::size_t _number,
                              const std::string &_text,
                              const std::string &_function,
                              const std::string &_link)
        : core::io::Line(_number, _text),
          function(_function),
          link(_link) {}

    [[nodiscard]] bool operator==(const UnlistedFunction &other) const
    {
        return number == other.number && text == other.text && function == other.function && link == other.link;
    }

    /**
     * @brief Unlisted function that needs to be added to include comments, prefixed with "std::" (e.g., "std::sort").
     */
    const std::string function;

    /**
     * @brief Link to the C++ reference for the function (e.g., "https://duckduckgo.com/?q=std%3A%3Asort+site%3Acppreference.com&ia=web").
     */
    const std::string link;
};

/**
 * @brief Class that extracts information from C++ code.
 *
 * On construction, the class loads the provided C++ file from disk, then extracts bare include directives (directives without any functions listed as comments), functions that are listed in comments but unused in the code, and functions that are used in the code but not listed as comments in any include directive. These results are accessible via getter functions.
 *
 * @note This class is marked as `final` to prevent inheritance.
 */
class CodeParser final {
  public:
    /**
     * @brief Construct a new CodeParser object.
     *
     * @param input_path Path to the C++ file that shall be parsed (e.g., "~/main.cpp").
     */
    explicit CodeParser(const std::string &input_path);

    /**
     * @brief Get a vector of bare include directives, i.e., without any standard functions listed after them as comments.
     *
     * @return Const reference to a vector of "BareInclude".
     */
    [[nodiscard]] const std::vector<BareInclude> &get_bare_includes() const;

    /**
     * @brief Get a vector of include directives with listed functions that may not be used in the code.
     *
     * @return Const reference to a vector of "IncludeWithUnusedFunctions".
     */
    [[nodiscard]] const std::vector<IncludeWithUnusedFunctions> &get_unused_functions() const;

    /**
     * @brief Get a vector of functions used in the code but not listed as comments after any include directive.
     *
     * @return Const reference to a vector of "UnlistedFunction".
     */
    [[nodiscard]] const std::vector<UnlistedFunction> &get_unlisted_functions() const;

  private:
    /**
     * @brief Vector of bare include directives, i.e., without any standard functions listed after them as comments.
     */
    std::vector<BareInclude> bare_includes_;

    /**
     * @brief Vector of include directives with unused functions, i.e., a list of standard functions that are not used in the code.
     */
    std::vector<IncludeWithUnusedFunctions> unused_functions_;

    /**
     * @brief Vector of unlisted standard functions, i.e., functions used in the code but not listed as comments after an include directive.
     */
    std::vector<UnlistedFunction> unlisted_functions_;
};

}  // namespace modules::analyze
