# header-warden

header-warden is a command-line tool that identifies and reports missing standard library headers in your C++ code.

![Knight surrounded by floating source code](/assets/hero.jpeg)

Image generated using OpenAI's [DALL-E 3](https://openai.com/dall-e-3).


## Motivation

In many modern programming languages, such as Python, the use of a standard library module is explicitly declared, making it clear which module is being utilized.

```python
import typing

def foo(bar: typing.List[int]) -> None:
    pass
```

**Note:** In [Python >=3.9](https://docs.python.org/3/whatsnew/3.9.html#type-hinting-generics-in-standard-collections), `list[]` can be used directly without using `typing.List`. This is merely for illustration purposes.


However, in C++, it is possible to use a standard library function without including the corresponding header.

```c++
// #include <vector>
#include <iostream>

void foo(std::vector<int> bar)  // No error or warning is generated
{
}
```

This is because they all share the `std` namespace and headers (e.g., `#include <iostream>`) can include other headers (e.g., `#include <vector>`) internally. This, however, varies between compilers and platforms, which can lead to portability issues (i.e., code that compiles on one platform but not on another).

As a C++ programmer, you're supposed to memorize *which* standard library functions are included in *which* headers. If you're a perfectionist like me, this quickly becomes very tedious and error-prone.

header-warden addresses this by encouraging the explicit listing of all standard library functions as comments in the `#include` directive. As a bonus, this also makes it easier to memorize the required headers for each function for beginners.

```c++
#include <algorithm>  // for std::find
#include <string>     // for std::string, std::to_string
#include <vector>     // for std::vector
```

After running header-warden, you will receive a report that lists all standard library functions used in your code, along with the corresponding `#include` directives. This will help you ensure that all standard library functions are correctly listed as comments after the `#include` directive. Links to [cppreference.com](https://en.cppreference.com/) are also provided for missing functions, which makes it easy to find the correct header.

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

What you do with this information is completely up to you. You can choose to add the missing functions to the comments, or you can ignore them. The goal is to make you aware of the potential issues in your code.


## Features

- Written in modern C++ (C++17).
- Comprehensive documentation with doxygen-style comments.
- No third-party dependencies.
- Automatic generation of links to [cppreference.com](https://en.cppreference.com/) for standard library functions.


## Project Structure

The `src` directory is organized as follows:

- `core`: Contains the most basic, primitive, standalone functions that do not depend on any other part of the application.
- `utils`: Contains utility functions that, while not as low-level as `core`, are used across different parts of the application.
- `io`: Manages input/output operations.


## Tested Systems

This project has been tested on the following systems:

- MacOS 14.4 (Sonoma)
- Debian 12 (Bookworm)
- Manjaro 23.1 (Vulcan)


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


## Install

If not already built, follow the steps in the [Build](#build) section and ensure that you are in the `build` directory.

To install the program, use the following command:

```bash
sudo cmake --install .
```

Consider adding an alias to your `.zsh_aliases` to recursively search for all C++ files in the current directory:

```bash
echo 'alias header-warden-recursive="find . -name \"*.cpp\" -o -name \"*.hpp\" -o -name \"*.h\" | xargs header-warden"' >> ~/.zsh_aliases
```


## Usage

To analyze a file, pass its filepath as a command line argument. Each file will be analyzed separately.

```bash
header-warden example.cpp
```

```
2024-03-28 03:37:23 | INFO    | main:39 - ##- example.cpp -##

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
header-warden *.cpp
header-warden *.hpp
header-warden *.h
header-warden *.*
```

You can also use the `find` command to recursively search for all C++ files.

```bash
find ../src -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs header-warden
find include -name "*.hpp" | xargs header-warden
```

If no arguments are provided (or `--help|-h` flag is used), the program will print a help message.

```bash
header-warden --help
```

If a verbose (`--verbose|-v`) flag is provided, the program outputs debug information, including the comparison of before, and after removing whitespace, categorization of lines, and the encoding of the function names when creating URLs.

```bash
header-warden main.cpp --verbose
```

Optionally, the `--disable-bare`, `--disable-unused`, and `--disable-unlisted` flags can be used to suppress messages about bare includes, unused functions, and unlisted functions, respectively. Any combination of these flags can be used. If you use all three flags, the program will not output any messages.

```bash
header-warden main.cpp --disable-unlisted --disable-unused
```

The `--disable-bare` flag can be useful when you're using external libraries (for example, `#include <fmt/core.h>`), and are annoyed by the messages about bare includes. This is a known limitation of the program, as it cannot distinguish between standard library headers (e.g., `#include <iostream>`) and external headers (e.g., `#include <fmt/core.h>`). I don't know how to programmatically distinguish between the two while being 100% certain that the header is part of the standard library.

```bash
header-warden main.cpp --disable-bare
```


## Contributing

All contributions are welcome.


## License

This project is licensed under the MIT License.
