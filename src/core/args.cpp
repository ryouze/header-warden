/**
 * @file args.cpp
 */

#include <exception>      // for std::exception
#include <filesystem>     // for std::filesystem
#include <string>         // for std::string
#include <unordered_set>  // for std::unordered_set
#include <vector>         // for std::vector

#include <argparse/argparse.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "args.hpp"
#include "version.hpp"

core::args::Args::Args(const int argc,
                       char **argv)
{
    // Define a set of common C++ file extensions
    // TODO: Add a way to manually override this set using a command-line argument
    const std::unordered_set<std::string> file_extensions = {".cpp", ".hpp", ".cxx", ".cc", ".hh", ".hxx", ".tpp"};

    // Define paths to be extracted from command-line arguments
    std::vector<std::string> files_or_directories;

    // Initialize ArgumentParser
    argparse::ArgumentParser program("header-warden", PROJECT_VERSION);
    program.set_usage_max_line_width(80);
    program.add_description("Identify and report missing headers in C++ code.");

    // Add positional arguments
    program.add_argument("paths")
        .help("files or directories to process")
        .nargs(argparse::nargs_pattern::at_least_one)
        .store_into(files_or_directories);

    // Add optional arguments
    program.add_argument("--no-bare")
        .help("disables bare include directives")
        .flag();

    program.add_argument("--no-unused")
        .help("disables unused functions")
        .flag();

    program.add_argument("--no-unlisted")
        .help("disables unlisted functions")
        .flag();

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &e) {
        throw ArgsError(fmt::format("Error: {}\n\n{}", e.what(), program.help().str()));
    }

    // Set enable's flags to true if the flag is not present
    // This is faster, because "store_into()" would set the flag to false by default, so we'd need to invert it afterwards
    this->enable.bare = program["--no-bare"] == false;
    this->enable.unused = program["--no-unused"] == false;
    this->enable.unlisted = program["--no-unlisted"] == false;

    // Process each path provided by the user
    for (const auto &filepath : files_or_directories) {
        // Get the current iteration as a normalized path
        const std::filesystem::path resolved_filepath = std::filesystem::absolute(filepath).lexically_normal();
        // Throw if doesn't exist
        if (!std::filesystem::exists(resolved_filepath)) {
            throw ArgsError(fmt::format("Error: Path does not exist: {}\n\n{}", resolved_filepath.string(), program.help().str()));
        }

        // If the path is a directory, recursively find all C++ files
        if (std::filesystem::is_directory(resolved_filepath)) {
            for (const auto &entry : std::filesystem::recursive_directory_iterator(resolved_filepath)) {
                // Throw if odesn't exist
                if (!entry.exists()) {
                    throw ArgsError(fmt::format("Error: Path does not exist: {}\n\n{}", entry.path().string(), program.help().str()));
                }
                // Append only if the file extension matches any of the C++ file types
                if (file_extensions.find(entry.path().extension().string()) != file_extensions.cend()) {
                    this->filepaths.emplace_back(entry.path());
                }
            }
        }
        // Otherwise, use the file path directly
        else {
            // Append only if the file extension matches any of the C++ file types
            if (file_extensions.find(resolved_filepath.extension().string()) != file_extensions.cend()) {
                this->filepaths.emplace_back(resolved_filepath);
            }
        }
    }

    // Throw if no C++ files were found
    if (this->filepaths.empty()) {
        // fmt can print a set directly, but fmt::join will prevent it from adding curly braces
        throw ArgsError(fmt::format("Error: No C++ files ({}) found in provided paths: {}\n\n{}", fmt::join(file_extensions, ", "), fmt::join(files_or_directories, ", "), program.help().str()));
    }
}
