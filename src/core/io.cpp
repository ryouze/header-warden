/**
 * @file io.cpp
 */

#include <cstddef>     // for std::size_t
#include <exception>   // for std::exception
#include <filesystem>  // for std::filesystem
#include <fstream>     // for std::ifstream
#include <stdexcept>   // for std::runtime_error
#include <string>      // for std::string, std::getline
#include <vector>      // for std::vector

#include <fmt/core.h>

#include "io.hpp"

namespace core::io {

std::vector<Line> read_lines(const std::filesystem::path &input_path,
                             const std::size_t initial_capacity)
{
    try {
        // Open the file in read mode
        std::ifstream file(input_path);

        // Error: File cannot be opened
        if (!file) {
            throw std::runtime_error("Failed to open file for reading");
        }

        // Reserve space for lines based on initial capacity
        std::vector<Line> lines;
        lines.reserve(initial_capacity);

        {  // Allocate buffer for reading lines
            std::size_t line_number = 0;
            std::string buffer;

            // Read the file line by line, incrementing the line number
            while (std::getline(file, buffer)) {
                lines.emplace_back(Line(++line_number, buffer));
            }
        }  // Deallocate buffer

        // Return shrunk vector (RVO)
        lines.shrink_to_fit();
        return lines;
    }
    catch (const std::exception &e) {
        throw std::runtime_error(fmt::format("Error loading file '{}': {}", input_path.string(), e.what()));
    }
}

}  // namespace core::io
