# header-warden

header-warden is a command-line tool that identifies and reports missing standard library headers in your C++ code.

![Knight surrounded by floating source code](/assets/warden.jpeg)

Image generated using OpenAI's [DALL-E 3](https://openai.com/dall-e-3).


## Motivation

In many modern programming languages, such as Python, the use of a standard library module is explicitly declared, making it clear which module is being utilized.

```python
import typing

def foo(bar: typing.List[int]) -> None:
    pass
```

**Note:** In [Python >=3.9](https://docs.python.org/3/whatsnew/3.9.html#type-hinting-generics-in-standard-collections), `list[]` can be used directly without using `typing.List`. This is merely for illustration purposes.


However, in C++, it is possible to use a standard library function without including the corresponding header. This is because they all share the `std` namespace.

```c++
// #include <vector>
#include <iostream>

void foo(std::vector<int> bar)  // No error or warning is generated
{
}
```

This can lead to portability issues as your code may fail to compile on a different platform or with a different compiler.

As a C++ programmer, you're supposed to memorize *which* standard library functions are included in *which* headers. If you're a perfectionist like me, this quickly becomes very tedious and error-prone.

header-warden addresses this by encouraging the explicit listing of all standard library functions as comments in the `#include` directive.

```c++
#include <algorithm>  // for std::find
#include <string>     // for std::string, std::to_string
#include <vector>     // for std::vector
```

After running header-warden, you will receive a report that lists all standard library functions used in your code, along with the corresponding `#include` directives. This will help you ensure that all standard library functions are correctly listed as comments after the `#include` directive. Links to [cppreference.com](https://en.cppreference.com/) are also provided for missing functions, which makes it easy to find the correct header.

What you do with this information is completely up to you. You can choose to add the missing functions to the comments, or you can ignore them. The goal is to make you aware of the potential issues in your code.

```
-- 1) BARE INCLUDES --

8| #include <iostream>
-> Bare include directive.
-> Add a comment to '#include <iostream>' that lists which functions depend on it, e.g., '#include <iostream>' // for std::foo, std::bar'.

-- 2) UNUSED FUNCTIONS --

11| #include <algorithm>  // for std::find
-> Unused functions listed as comments.
-> Remove the following functions from comments of the '#include <algorithm>' include directive: ["std::find"]

-- 3) UNLISTED FUNCTIONS --

31|     std::sort(result.begin(), result.end());
-> Unlisted function.
-> Add 'std::sort' as a comment to the include directives, e.g., "#include <foo> // for std::sort"
-> Reference: https://duckduckgo.com/?sites=cppreference.com&q=std%3A%3Asort&ia=web
```

**Note:** The program' only analyzes comments that are directly associated with an include directive. For instance, if there's a comment in a line such as `int x = 5; // Use std::cout to print it`, the `std::cout` within the comment will not be taken into account by the program. It only becomes relevant if it's linked with an include directive, like in `#include <iostream>  // for std::cout`. Similarly, lines that are commented out, such as `// std::cerr << "ERROR!\n";`, or comments in documentation format like `* @param is_verbose If true, print with std::cout.`, are also ignored by the program. This approach ensures that your commented out code or documentation comments do not lead to false positives.


## Features

- Written in modern C++ (C++17).
- Comprehensive documentation with doxygen-style comments.
- No third-party dependencies (with custom thread-safe logger and argument parser).
- Automatic generation of links to [cppreference.com](https://en.cppreference.com/) for standard library functions.


## Tested Systems

This project has been tested on the following systems:

- MacOS 14.4 (Sonoma)
- Debian 12 (Bookworm)


## Requirements

To build and run this project, you'll need:

- C++17 or higher
- CMake


## Build

Follow these steps to build the project:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/ryouze/header-warden.git
   ```

2. **Generate the build system**:
   ```bash
   cd header-warden
   mkdir build && cd build
   cmake ..
   ```

3. **Compile the project**:
    ```bash
    make -j
    ```

After successful compilation, you can run the program using `./header-warden`.

The mode is set to `Release` by default. To build in `Debug` mode, use `cmake -DCMAKE_BUILD_TYPE=Debug ..`.


## Usage

To analyze a file, pass its filepath as a command line argument. Each file will be analyzed separately.

```bash
./header-warden main.cpp
```

```
2024-03-28 03:37:23 | INFO    | main:39 - ##- ../example.cpp -##

-- 1) BARE INCLUDES --

8| #include <iostream>
-> Bare include directive.
-> Add a comment to '#include <iostream>' that lists which functions depend on it, e.g., '#include <iostream>' // for std::foo, std::bar'.

-- 2) UNUSED FUNCTIONS --

11| #include <algorithm>  // for std::find
-> Unused functions listed as comments.
-> Remove the following functions from comments of the '#include <algorithm>' include directive: ["std::find"]

-- 3) UNLISTED FUNCTIONS --

-> No unlisted functions found.

--------------------------------------------------------------------------------
```

You can also use wildcards to analyze multiple files at once.

```bash
./header-warden *.cpp
./header-warden *.hpp
./header-warden *.h
./header-warden */*.*
```

Or you can specify the path to the files.

```bash
./header-warden ../src/*/*.*
```

If no arguments are provided (or `--help|-h` flag is used), the program will print the help message.

```bash
./header-warden --help
```

If a verbose (`--verbose|-v`) flag is provided, the program outputs debug information, including the comparison of before, and after removing whitespace, categorization of lines, and the encoding of the function names when creating URLs.

```bash
./header-warden main.cpp --verbose
```

## Contributing

All contributions are welcome.


## License

This project is licensed under the MIT License.
