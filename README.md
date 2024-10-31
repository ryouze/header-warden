# header-warden

[![CI](https://github.com/ryouze/header-warden/actions/workflows/ci.yml/badge.svg)](https://github.com/ryouze/header-warden/actions/workflows/ci.yml)
[![Release](https://github.com/ryouze/header-warden/actions/workflows/release.yml/badge.svg)](https://github.com/ryouze/header-warden/actions/workflows/release.yml)
![Release version](https://img.shields.io/github/v/release/ryouze/header-warden)

header-warden is a cross-platform CLI tool that identifies and reports missing standard library headers in C++ code.


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
-> Add a comment to '#include <iostream>', e.g., '#include <iostream> // for std::foo, std::bar'.

-- 2) UNUSED FUNCTIONS --

11| #include <algorithm>  // for std::find
-> Unused functions listed as comments.
-> Remove 'std::find' comments from '#include <algorithm>  // for std::find'.

-- 3) UNLISTED FUNCTIONS --

31|     std::sort(result.begin(), result.end());
-> Unlisted function.
-> Add 'std::sort' as a comment, e.g., '#include <foo> // for std::sort'.
-> Reference: https://duckduckgo.com/?sites=cppreference.com&q=std%3A%3Asort&ia=web
```

What you do with this information is completely up to you. You can choose to add the missing functions to the comments, or you can ignore them. The goal is to make you aware of the potential issues in your code.


## Features

- Written in modern C++ (C++17).
- Comprehensive documentation with doxygen-style comments.
- Automatic third-party dependency management using CMake's [FetchContent](https://www.foonathan.net/2022/06/cmake-fetchcontent/).
- No missing STL headers thanks to this tool (run on itself).


## Tested Systems

This project has been tested on the following systems:

- macOS 14.6 (Sonoma)
- Manjaro 24.0 (Wynsdey)
- Windows 11 23H2

Automated testing is also performed on the latest versions of macOS, GNU/Linux, and Windows using GitHub Actions.


## Pre-built Binaries

Pre-built binaries are available for macOS (ARM64), GNU/Linux (x86_64), and Windows (x86_64). You can download the latest version from the [Releases](../../releases) page.

To remove macOS quarantine, use the following commands:

```sh
xattr -d com.apple.quarantine header-warden-macos-arm64
chmod +x header-warden-macos-arm64
```


## Requirements

To build and run this project, you'll need:

- C++17 or higher
- CMake


## Build

Follow these steps to build the project:

1. **Clone the repository**:

    ```sh
    git clone https://github.com/ryouze/header-warden.git
    ```

2. **Generate the build system**:

    ```sh
    cd header-warden
    mkdir build && cd build
    cmake ..
    ```

    Optionally, you can disable compile warnings by setting `ENABLE_COMPILE_FLAGS` to `OFF`:

    ```sh
    cmake .. -DENABLE_COMPILE_FLAGS=OFF
    ```

3. **Compile the project**:

    To compile the project, use the following command:

    ```sh
    make
    ```

    If you want to use all available cores when compiling with `make`, you can pass the `-j` flag along with the number of cores available on your system:

    - **macOS**:

      ```sh
      make -j$(sysctl -n hw.ncpu)
      ```

    - **GNU/Linux**:

      ```sh
      make -j$(nproc)
      ```

    - **8-core CPU**:

      ```sh
      make -j8
      ```

After successful compilation, you can run the program using `./header-warden`. However, it is highly recommended to install the program, so that it can be run from any directory. Refer to the [Install](#install) section below.

**Note:** The mode is set to `Release` by default. To build in `Debug` mode, use `cmake .. -DCMAKE_BUILD_TYPE=Debug`.


## Install

If not already built, follow the steps in the [Build](#build) section and ensure that you are in the `build` directory.

To install the program, use the following command:

```sh
sudo cmake --install .
```

On macOS, this will install the program to `/usr/local/bin`. You can then run the program from any directory using `header-warden`.


## Usage

To run the program, use the following command:

```sh
header-warden
```

The program expects at least one argument, which can be a file or directory.

```sh
header-warden src/main.cpp
```

```sh
header-warden src
```

If a directory is passed, the program will search for files with the following extensions: `.cpp`, `.hpp`, `.h`, `.cxx`, `.cc`, `.hh`, `.hxx`, `.tpp`.

You can also pass multiple files and directories as arguments.

```sh
header-warden ../src ~/dev/app/tests
```

**Note:** On Windows, a modern terminal emulator like [Windows Terminal](https://github.com/microsoft/terminal) is recommended, although the default Command Prompt will display UTF-8 characters correctly.


## Flags

```sh
[~] $ header-warden --help
Usage: header-warden [--help] [--version] [--no-bare] [--no-unused]
                     [--no-unlisted]
                     paths...

Identify and report missing headers in C++ code.

Positional arguments:
  paths          files or directories to process [nargs: 1 or more]

Optional arguments:
  -h, --help     shows help message and exits
  -v, --version  prints version information and exits
  --no-bare      disables bare include directives
  --no-unused    disables unused functions
  --no-unlisted  disables unlisted functions
```


## Testing

Tests are included in the project but are not built by default.

To enable and build the tests manually, run the following commands:

```sh
cmake .. -DBUILD_TESTS=ON
make
ctest --output-on-failure
```


## Credits

- [argparse](https://github.com/p-ranav/argparse)
- [fmt](https://github.com/fmtlib/fmt)


## Contributing

All contributions are welcome.


## License

This project is licensed under the MIT License.
