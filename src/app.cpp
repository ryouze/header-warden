/**
 * @file app.cpp
 */

#include <filesystem>  // for std::filesystem
#include <sstream>     // for std::ostringstream

#include <BS_thread_pool.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "app.hpp"
#include "core/args.hpp"
#include "core/string.hpp"
#include "modules/analyze.hpp"

namespace app {

void run(const core::args::Args &args)
{
    const std::size_t filepaths_len = args.filepaths.size();
    fmt::print("Analyzing {} files: [{}]\n\n",
               filepaths_len,
               fmt::join(core::string::paths_to_strings(args.filepaths), ", "));
    // fmt::print("Enabled: bare={}, unused={}, unlisted={}, multithreading={}\n\n\n",
    //            args.enable.bare, args.enable.unused, args.enable.unlisted, args.enable.multithreading);

    fmt::print("--------------------------------------------------------------------------------\n\n");

    // Create a synced stream for thread-safe printing
    // Tip: Always create the "BS::synced_stream" object before the "BS::thread_pool" object to avoid crashes
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
            oss << "-- 1) BARE INCLUDES --\n\n";
            if (args.enable.bare) {
                for (const auto &entry : bare_includes) {
                    oss << fmt::format("{}| {}\n", entry.number, entry.text);
                    oss << "-> Bare include directive.\n";
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
            oss << "-- 2) UNUSED FUNCTIONS --\n\n";
            if (args.enable.unused) {
                for (const auto &entry : unused_functions) {
                    oss << fmt::format("{}| {}\n", entry.number, entry.text);
                    oss << "-> Unused functions listed as comments.\n";
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
            oss << "-- 3) UNLISTED FUNCTIONS --\n\n";
            if (args.enable.unlisted) {
                for (const auto &entry : unlisted_functions) {
                    oss << fmt::format("{}| {}\n", entry.number, entry.text);
                    oss << "-> Unlisted function.\n";
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

    if (filepaths_len < 2 || !args.enable.multithreading) {
        // Sequential processing for less than 2 files or if multithreading is disabled
        // fmt::print("Processing files sequentially...\n\n");
        for (const auto &path : args.filepaths) {
            process_file(path);
        }
    }
    else {
        // Parallel processing for 2 or more files
        // Create a thread pool with as many threads as are available in the hardware
        BS::thread_pool pool;

        // Collect futures in a BS::multi_future, preallocating space for all filepaths
        BS::multi_future<void> futures;
        futures.reserve(filepaths_len);

        // Process each filepath in parallel
        // fmt::print("Processing files in parallel...\n\n");
        for (const auto &path : args.filepaths) {
            // Submit a task to the thread pool and emplace the future
            // Tip: "submit_task()" catches any exceptions thrown by the submitted task and forwards them to the corresponding future, which can be caught using the "get()" member function of the future object
            futures.emplace_back(pool.submit_task([&process_file, &path]() {
                process_file(path);
            }));
        }

        // Wait for all tasks to complete and rethrow exceptions
        // Tip: "wait()" does not throw any exceptions; only "get()" does, so even if your task does not return anything, i.e., your future is an "std::future<void>", you must still use "get()" on the future obtained from it if you want to catch exceptions thrown by it
        futures.get();
    }
}

}  // namespace app
