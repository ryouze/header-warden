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

```
--------------------------------------------------------------------------------
##- example.cpp: BARE INCLUDES -##

2| #include <iostream>
-> Bare include directive.
-> Add a comment to "#include <iostream>" that lists which functions depend on it, e.g., "#include <iostream> // for std::foo, std::bar".

--------------------------------------------------------------------------------
##- example.cpp: UNUSED FUNCTIONS -##

1| #include <cstdlib>  // for std::exit, std::test, std::test2
-> Unused functions listed as comments.
-> Remove the following functions from comments of the "#include <cstdlib>" include directive: "std::test", "std::test2".

--------------------------------------------------------------------------------
##- example.cpp: UNLISTED FUNCTIONS -##

4| std::vector<int> foo()
-> Unlisted function.
-> Add "std::vector" as a comment to the include directives, e.g., "#include <foo> // for std::vector".
-> Reference: https://duckduckgo.com/?sites=cppreference.com&q=std%3A%3Avector&ia=web
```

**Note:** The program' only analyzes comments that are directly associated with an include directive. For instance, if there's a comment in a line such as `int x = 5; // Use std::cout to print it`, the `std::cout` within the comment will not be taken into account by the program. It only becomes relevant if it's linked with an include directive, like in `#include <iostream>  // for std::cout`. Similarly, lines that are commented out, such as `// std::cerr << "ERROR!\n";`, or comments in documentation format like `* @param is_verbose If true, print with std::cout.`, are also ignored by the program. This approach ensures that your commented out code or documentation comments do not lead to false positives.


## Features

- Written in modern C++ (C++17).
- Comprehensive documentation with doxygen-style comments.
- No third-party dependencies.
- Automatic generation of links to [cppreference.com](https://en.cppreference.com/) for standard library functions.


## Tested Systems

This project has been tested on the following systems:

- MacOS 14.3 (Sonoma)


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
--------------------------------------------------------------------------------
##- example.cpp: BARE INCLUDES -##

--------------------------------------------------------------------------------
##- example.cpp: UNUSED FUNCTIONS -##

6| #include <string>     // for std::string, std::to_string
-> Unused functions listed as comments.
-> Remove the following functions from comments of the "#include <string>" include directive: "std::to_string".

--------------------------------------------------------------------------------
##- ../src/args.cpp: UNLISTED FUNCTIONS -##

71|     std::copy_if(this->args_.begin(), this->args_.end(), std::back_inserter(positional_args),
-> Unlisted function.
-> Add "std::back_inserter" as a comment to the include directives, e.g., "#include <foo> // for std::back_inserter".
-> Reference: https://duckduckgo.com/?sites=cppreference.com&q=std%3A%3Aback_inserter&ia=web
```

You can also use wildcards to analyze multiple files at once.

```bash
./header-warden *.cpp
./header-warden *.hpp
./header-warden *.h
```

If no arguments are provided (or `--help` flag is used), the program will print the help message.

```bash
./header-warden --help
```

If a verbose (`--verbose`) flag is provided, the program outputs the full code with line-by-line categorization. This is useful for debugging.

```bash
./header-warden example.cpp --verbose
```

```
--------------------------------------------------------------------------------
##- example.cpp: SOURCE CODE -##

1| #include <cstdlib>  // for std::exit, std::test, std::test2
-> Include directive with listed functions as a comment.
2| #include <iostream>
-> Bare include directive.
3|
-> Nothing.
4| std::vector<int> foo()
-> Standard library functions.
5| {
-> Nothing.
6|     return std::vector<int>{1, 2, 3};
-> Standard library functions.
7| }
-> Nothing.
8|
-> Nothing.
9| std::exit(exit_failure);
-> Standard library functions.
10| ;
-> Nothing.

--------------------------------------------------------------------------------
##- example.cpp: BARE INCLUDES -##

2| #include <iostream>
-> Bare include directive.
...
```

## Contributing

All contributions are welcome.


## License

This project is licensed under the MIT License.
