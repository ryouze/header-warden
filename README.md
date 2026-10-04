# header-warden

[![CI](https://github.com/ryouze/header-warden/actions/workflows/ci.yml/badge.svg)](https://github.com/ryouze/header-warden/actions/workflows/ci.yml)
[![Release](https://github.com/ryouze/header-warden/actions/workflows/release.yml/badge.svg)](https://github.com/ryouze/header-warden/actions/workflows/release.yml)
![Release version](https://img.shields.io/github/v/release/ryouze/header-warden)

header-warden is a cross-platform, multithreaded CLI tool that identifies and reports missing standard library headers in C++ code.

## Motivation

In many modern programming languages, such as Python, the use of a standard library module is explicitly declared, making it clear which module is being used.

```python
import typing

def foo(bar: typing.List[int]) -> None:
    pass
```

> [!NOTE]
> In [Python >=3.9](https://docs.python.org/3/whatsnew/3.9.html#type-hinting-generics-in-standard-collections), `list[]` can be used directly without using `typing.List`. This is merely for illustration purposes.

However, in C++, it is possible to use a standard library function without including the corresponding header.

```c++
// #include <vector>
#include <iostream>

void foo(std::vector<int> bar)  // No error or warning is generated
{
}
```

This is because standard library functions and types share the `std` namespace, and headers (e.g., `#include <iostream>`) can internally include other headers (e.g., `#include <vector>`). These indirect includes can vary between compilers and platforms, which can lead to portability issues (i.e., code that compiles on one platform but not another).

As a C++ programmer, you're expected to know which standard library functions and types require which headers. If you're a perfectionist like me, this quickly becomes tedious and error-prone.

header-warden addresses this by encouraging you to explicitly list all standard library functions and types used from each header in comments next to the corresponding `#include` directive. As a bonus, this also makes it easier for beginners to learn which headers provide which functions and types.

```c++
#include <algorithm>  // for std::find
#include <string>     // for std::string, std::to_string
#include <vector>     // for std::vector
```

After running header-warden, you will receive a report listing standard library functions and types used in your code that are missing from these comments, as well as bare includes and comments for unused functions. Links to [cppreference.com](https://en.cppreference.com/) are also provided for unlisted functions, making it easy to find the correct header.

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

What you do with this information is up to you. You can add the missing functions and types to the comments or ignore them. The goal is to make you aware of potential issues in your code.

## Features

- Written in modern C++ (C++17).
- Multithreaded file processing via [BS::thread_pool](https://github.com/bshoshany/thread-pool).

## Tested Systems

This project has been tested on the following systems:

- macOS 14.6 (Sonoma)
- Manjaro 24.0 (Wynsdey)
- Windows 11 23H2

Automated tests are also run on the latest versions of macOS, GNU/Linux, and Windows using GitHub Actions.

## Pre-built Binaries

Pre-built binaries are available for macOS (ARM64), GNU/Linux (x86_64), and Windows (x86_64). You can download the latest version from the [Releases](../../releases) page.

To remove the quarantine attribute on macOS, use the following commands:

```sh
xattr -d com.apple.quarantine header-warden-macos-arm64
chmod +x header-warden-macos-arm64
```

On Windows, the OS may warn you that the binary is unsigned. You can bypass this warning by clicking "More info" and then "Run anyway".

## Requirements

To build and run this project, you'll need:

- C++17 or higher
- CMake

## Build

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

   Optionally, you can disable compiler warnings by setting `ENABLE_COMPILE_FLAGS` to `OFF`:

   ```sh
   cmake .. -DENABLE_COMPILE_FLAGS=OFF
   ```

3. **Compile the project**:

   ```sh
   cmake --build . --parallel
   ```

After a successful build, you can run the program using `./header-warden`. However, installing the program is recommended so that it can be run from any directory. See the [Install](#install) section below.

> [!TIP]
> The build type is set to `Release` by default. To build in `Debug` mode, use `cmake .. -DCMAKE_BUILD_TYPE=Debug`.

## Install

If you haven't already built the project, follow the steps in the [Build](#build) section and make sure you are in the `build` directory.

To install the program, use the following command:

```sh
sudo cmake --install .
```

On macOS, this installs the program to `/usr/local/bin`. You can then run it from any directory using `header-warden`.

## Usage

The program expects at least one argument, which can be a file or directory:

```sh
header-warden src/main.cpp
```

```sh
header-warden src
```

If a directory is passed, the program recursively searches for files with the following extensions: `.cpp`, `.hpp`, `.h`, `.cxx`, `.cc`, `.hh`, `.hxx`, and `.tpp`.

You can also pass multiple files and directories:

```sh
header-warden ../src ~/dev/app/tests
```

> [!TIP]
> On Windows, a modern terminal emulator like [Windows Terminal](https://github.com/microsoft/terminal) is recommended, although the default Command Prompt displays UTF-8 characters correctly.

## Flags

```sh
[~] $ header-warden --help
Usage: header-warden [--help] [--version] [--no-bare] [--no-unused]
                     [--no-unlisted] [--no-multithreading]
                     paths...

Identify and report missing headers in C++ code.

Positional arguments:
  paths                files or directories to process [nargs: 1 or more]

Optional arguments:
  -h, --help           shows help message and exits
  -v, --version        prints version information and exits
  --no-bare            disables bare include directives
  --no-unused          disables unused functions
  --no-unlisted        disables unlisted functions
  --no-multithreading  disables multithreading
```

## Testing

Tests are included in the project but are not built by default.

To enable and build the tests manually, run the following commands from the `build` directory:

```sh
cmake .. -DBUILD_TESTS=ON
cmake --build . --parallel
ctest --output-on-failure
```

## Credits

- [argparse](https://github.com/p-ranav/argparse)
- [BS::thread_pool](https://github.com/bshoshany/thread-pool)
- [fmt](https://github.com/fmtlib/fmt)
