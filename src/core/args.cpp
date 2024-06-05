/**
 * @file args.cpp
 */

#include <algorithm>  // for std::find, std::copy_if
#include <iterator>   // for std::back_inserter
#include <string>     // for std::string
#include <vector>     // for std::vector

#include "args.hpp"

core::args::ArgParser::ArgParser(const int argc,
                                 char **argv)
{
    // Store the program name (e.g., "./bin")
    this->program_name_ = argv[0];

    // Convert the C-style arguments to a string vector, starting from 1 to exclude the program name
    this->args_ = std::vector<std::string>(argv + 1, argv + argc);
    this->args_.shrink_to_fit();
}

bool core::args::ArgParser::empty() const
{
    return this->args_.empty();
}

bool core::args::ArgParser::contains(const std::string &arg) const
{
    // Use std::find to search for 'arg' in 'args_'.
    // -> If 'arg' is found, return an iterator pointing to the first occurrence of 'arg'.
    // -> If 'arg' is not found, return 'args_.cend()'.
    // Then, check if the returned iterator is not equal to 'args_.cend()' (if the 'arg' was found in 'args_').
    return (std::find(this->args_.cbegin(), this->args_.cend(), arg) != this->args_.cend());
}

bool core::args::ArgParser::contains(const std::string &short_arg,
                                     const std::string &long_arg) const
{
    return (this->contains(short_arg) || this->contains(long_arg));
}

// const std::string &core::args::ArgParser::value(const std::string &arg) const
// {
//     // Find the iterator pointing to the given keyword in the arguments (e.g., "--file")
//     std::vector<std::string>::const_iterator itr = std::find(this->args_.cbegin(), this->args_.cend(), arg);

//     // If the keyword is found (e.g., "--file") and there is an element after it (e.g., "data.txt"), return the next element as the value (e.g., "data.txt")
//     if (itr != this->args_.cend() && ++itr != this->args_.cend()) {
//         return *itr;
//     }

//     // If the keyword is not found or there is no element after it, throw an exception
//     throw std::runtime_error("The keyword argument '" + arg + "' does not contain a value (e.g., `--file 'data.txt'`)");
// }

// const std::string &core::args::ArgParser::value(const std::string &short_arg,
//                                                 const std::string &long_arg) const
// {
//     // Try to find the value of the short argument
//     try {
//         return this->value(short_arg);
//     }
//     // If the short argument is not found, try to find the value of the long argument
//     catch (std::runtime_error &e) {
//         return this->value(long_arg);
//     }
// }

// const std::string &core::args::ArgParser::position(const std::size_t index) const
// {
//     // If the index is out of range, throw an exception
//     if (index >= this->args_.size()) {
//         throw std::out_of_range("The requested index of the positional argument '" + std::to_string(index) + "' is out of range");
//     }

//     // Return a reference to the value of the argument at the given index
//     // We already checked for out-of-range index, so it's safe to use the [] operator
//     return this->args_[index];
// }

// const std::vector<std::string> &core::args::ArgParser::get_arguments() const
// {
//     return this->args_;
// }

std::vector<std::string> core::args::ArgParser::get_positional_arguments() const
{
    // Create a vector to store positional arguments
    std::vector<std::string> positional_args;

    // Use std::copy_if to copy arguments that do not begin with "-" or "--"
    std::copy_if(this->args_.cbegin(), this->args_.cend(), std::back_inserter(positional_args),
                 [](const std::string &arg) {
                     // Check if the argument does not start with "-" and does not start with "--"
                     return arg.empty() || (arg[0] != '-' && !(arg.size() > 1 && arg[1] == '-'));
                 });

    // Return shrunk vector of positional arguments (RVO)
    positional_args.shrink_to_fit();
    return positional_args;
}

std::string core::args::ArgParser::get_help() const
{
    return "Usage: " + this->program_name_ +
           " [OPTIONS]... [FILE]...\n\n"
           "Description:\n"
           "  Find missing standard library headers in C++ files.\n\n"
           "Options:\n"
           "  -h, --help                  Display this help message and exit.\n"
           "  -v, --verbose               Display detailed output.\n"
           "  --disable-bare              Suppress messages about bare includes.\n"
           "  --disable-unused            Suppress messages about unused functions.\n"
           "  --disable-unlisted          Suppress messages about unlisted functions.\n\n"
           "Arguments:\n"
           "  FILE                        One or more C++ files to analyze. Specify the path to each file.\n\n"
           "Examples:\n"
           "  " +
           this->program_name_ + " *.cpp\n"
                                 "  " +
           this->program_name_ + " main.cpp\n"
                                 "  " +
           this->program_name_ + " --disable-bare --disable-unused args.hpp src/*.hpp\n";
}
