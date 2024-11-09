/**
 * @file app.cpp
 */

#include <filesystem>  // for std::filesystem
#include <sstream>     // for std::ostringstream

#include <BS_thread_pool.hpp>
#include <BS_thread_pool_utils.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "app.hpp"
#include "core/args.hpp"
#include "core/string.hpp"
#include "modules/analyze.hpp"

namespace app {

void run(const core::args::Args &args)
{
    fmt::print("Analyzing {} files: [{}]\n\n",
               args.filepaths.size(),
               fmt::join(core::string::paths_to_strings(args.filepaths), ", "));
    // fmt::print("Enabled: bare={}, unused={}, unlisted={}\n\n", args.enable.bare, args.enable.unused, args.enable.unlisted);

    fmt::print("--------------------------------------------------------------------------------\n\n");

    // Create a synced stream for thread-safe printing
    BS::synced_stream sync_out;

    // Function to process a single file
    const auto process_file = [&args, &sync_out](const std::filesystem::path &path) {
        std::ostringstream oss;
        oss << fmt::format("##- {} -##\n\n", path.string());
        const modules::analyze::CodeParser parser(path);

        // Get references to the parser's extracted data / results
        const auto &bare_includes = parser.get_bare_includes();
        const auto &unused_functions = parser.get_unused_functions();
        const auto &unlisted_functions = parser.get_unlisted_functions();

        // Collect bare includes
        if (!bare_includes.empty()) {
            oss << fmt::format("-- 1) BARE INCLUDES --\n\n");
            if (args.enable.bare) {
                for (const auto &entry : bare_includes) {
                    oss << fmt::format("{}| {}\n", entry.number, entry.text);
                    oss << fmt::format("-> Bare include directive.\n");
                    oss << fmt::format("-> Add a comment to '{}', e.g., '{} // for std::foo, std::bar'.\n\n",
                                       entry.header, entry.header);
                }
            }
            else {
                oss << fmt::format("-> Disabled, but found {} bare include directives.\n\n",
                                   bare_includes.size());
            }
        }

        // Collect unused functions
        if (!unused_functions.empty()) {
            oss << fmt::format("-- 2) UNUSED FUNCTIONS --\n\n");
            if (args.enable.unused) {
                for (const auto &entry : unused_functions) {
                    oss << fmt::format("{}| {}\n", entry.number, entry.text);
                    oss << fmt::format("-> Unused functions listed as comments.\n");
                    oss << fmt::format("-> Remove '{}' comments from '{}'.\n\n",
                                       fmt::join(entry.unused_functions, "', '"), entry.text);
                }
            }
            else {
                oss << fmt::format("-> Disabled, but found {} unused functions.\n\n",
                                   unused_functions.size());
            }
        }

        // Collect unlisted functions
        if (!unlisted_functions.empty()) {
            oss << fmt::format("-- 3) UNLISTED FUNCTIONS --\n\n");
            if (args.enable.unlisted) {
                for (const auto &entry : unlisted_functions) {
                    oss << fmt::format("{}| {}\n", entry.number, entry.text);
                    oss << fmt::format("-> Unlisted function.\n");
                    oss << fmt::format("-> Add '{}' as a comment, e.g., '#include <foo> // for {}'.\n",
                                       entry.function, entry.function);
                    oss << fmt::format("-> Reference: {}\n\n", entry.link);
                }
            }
            else {
                oss << fmt::format("-> Disabled, but found {} unlisted functions.\n\n",
                                   unlisted_functions.size());
            }
        }

        // If nothing found, print OK
        if (bare_includes.empty() && unused_functions.empty() && unlisted_functions.empty()) {
            oss << "-> OK.\n\n";
        }

        oss << "--------------------------------------------------------------------------------\n\n";

        // Use synced stream to print the output
        sync_out.print(oss.str());
    };

    if (args.filepaths.size() < 2) {
        // Sequential processing for less than 2 files
        for (const auto &path : args.filepaths) {
            process_file(path);
        }
    }
    else {
        // Parallel processing for 2 or more files
        // Create a thread pool
        BS::thread_pool pool;

        // Collect futures in a BS::multi_future
        BS::multi_future<void> futures;

        // Process each filepath in parallel
        for (const auto &path : args.filepaths) {
            // Submit a task to the thread pool and emplace the future
            futures.emplace_back(pool.submit_task([path, &process_file]() {
                process_file(path);
            }));
        }

        // Wait for all tasks to complete and rethrow exceptions
        futures.get();
    }
}

}  // namespace app
