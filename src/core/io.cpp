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

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <locale>            // for setlocale, LC_ALL
#include <windows.h>         // for WideCharToMultiByte, GetLastError, CP_UTF8, SetConsoleCP, SetConsoleOutputCP
#endif

#include <fmt/core.h>

#include "io.hpp"

void core::io::setup_utf8_console()
{
#if defined(_WIN32)
    if (!SetConsoleCP(CP_UTF8) || !SetConsoleOutputCP(CP_UTF8)) {
        throw std::runtime_error(fmt::format("Failed to set UTF-8 code page: {}", GetLastError()));
    }

    if (!setlocale(LC_ALL, ".UTF8")) {
        throw std::runtime_error("Failed to set UTF-8 locale");
    }
#endif
}

std::vector<core::io::Line> core::io::read_lines(const std::filesystem::path &input_path,
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
        std::vector<core::io::Line> lines;
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
