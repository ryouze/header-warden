/**
 * @file app.cpp
 */

#include <fmt/core.h>
#include <fmt/ranges.h>

#include "app.hpp"
#include "core/args.hpp"
#include "core/string.hpp"
#include "modules/analyze.hpp"

void app::run(const int argc,
              char **argv)
{
    // Process command-line arguments (this might throw an ArgParseError)
    const core::args::Args args(argc, argv);

    fmt::print("Analyzing {} files: [{}]\n\n",
               args.filepaths.size(),
               fmt::join(core::string::paths_to_strings(args.filepaths), ", "));
    // fmt::print("Enabled: bare={}, unused={}, unlisted={}\n\n", args.enable.bare, args.enable.unused, args.enable.unlisted);

    fmt::print("--------------------------------------------------------------------------------\n\n");

    // Process each filepath
    for (const auto &path : args.filepaths) {
        fmt::print("##- {} -##\n\n", path.string());
        const modules::analyze::CodeParser parser(path);

        // Get references to the parser's extracted data / results
        const auto &bare_includes = parser.get_bare_includes();
        const auto &unused_functions = parser.get_unused_functions();
        const auto &unlisted_functions = parser.get_unlisted_functions();

        // Print bare includes
        if (!bare_includes.empty()) {
            fmt::print("-- 1) BARE INCLUDES --\n\n");
            if (args.enable.bare) {
                for (const auto &entry : parser.get_bare_includes()) {
                    fmt::print("{}| {}\n", entry.number, entry.text);
                    fmt::print("-> Bare include directive.\n");
                    fmt::print("-> Add a comment to '{}', e.g., '{} // for std::foo, std::bar'.\n\n", entry.header, entry.header);
                }
            }
            else {
                fmt::print("-> Disabled, but found {} bare include directives.\n\n", bare_includes.size());
            }
        }

        // Print unused functions
        if (!unused_functions.empty()) {
            fmt::print("-- 2) UNUSED FUNCTIONS --\n\n");
            if (args.enable.unused) {
                for (const auto &entry : parser.get_unused_functions()) {
                    fmt::print("{}| {}\n", entry.number, entry.text);
                    fmt::print("-> Unused functions listed as comments.\n");
                    fmt::print("-> Remove '{}' comments from '{}'.\n\n", fmt::join(entry.unused_functions, "', '"), entry.text);
                }
            }
            else {
                fmt::print("-> Disabled, but found {} unused functions.\n\n", unused_functions.size());
            }
        }

        // Print unlisted functions
        if (!unlisted_functions.empty()) {
            fmt::print("-- 3) UNLISTED FUNCTIONS --\n\n");
            if (args.enable.unlisted) {
                for (const auto &entry : parser.get_unlisted_functions()) {
                    fmt::print("{}| {}\n", entry.number, entry.text);
                    fmt::print("-> Unlisted function.\n");
                    fmt::print("-> Add '{}' as a comment, e.g., '#include <foo> // for {}'.\n",
                               entry.function, entry.function);
                    fmt::print("-> Reference: {}\n\n", entry.link);
                }
            }
            else {
                fmt::print("-> Disabled, but found {} unlisted functions.\n\n", unlisted_functions.size());
            }
        }

        // If nothing, found, print OK
        if (bare_includes.empty() && unused_functions.empty() && unlisted_functions.empty()) {
            fmt::print("-> OK.\n\n");
        }

        fmt::print("--------------------------------------------------------------------------------\n\n");
    }
}
