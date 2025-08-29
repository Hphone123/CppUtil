[![CI](https://github.com/Hphone123/CppUtil/actions/workflows/ci.yaml/badge.svg)](https://github.com/Hphone123/CppUtil/actions/workflows/ci.yaml)
[![Code Coverage](https://hphone123.github.io/CppUtil/badge.svg)](https://github.com/Hphone123/CppUtil/)

# CppUtil

C++ Utility library implementing whatever I may need right now.

## Prerequesits

### VCPKG

All dependencies are installed automaticly via [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started).
Make sure you have it installed, set up and `VCPKG_ROOT` is set correctly.

### Pre-commit

This repo uses clang-format v14.0.6 for styling. To automaticly style on every commit, use `pre-commit`:
``` sh
# Install python if you haven't already
sudo apt update && sudo apt install python3

pipx install pre-commit
pre-commit install

# Run the hook once to fetch required repos
pre-commit run -a
```

## Usage

### Build

Build using CMake:

``` sh
cmake -S . -B build [Options] [-DCMAKE_BUILD_TYPE=[Debug|Release|...]] [-DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"]
cmake --build build
```

#### Options

Option                  | Description                                                                       | Default
------------------------|-----------------------------------------------------------------------------------|---------
-DALLOW_TEST_FAIL       | Wether to allow the build to continue after a unit-test fail                      | OFF
-DRUN_TESTS_AFTER_BUILD | Wether to run unit tests on build time                                            | ON
-DCREATE_PCH            | Wether to create pre-compile heades (will speed up compile, may introduce errors) | ON
-DBUILD_TESTS           | Wether to build unit tests (requires catch2, via vcpkg or other source)           | ON
-DCHECK_COVERAGE        | Wether to create a test-coverage report                                           | OFF


### Test

Run tests using CTest:
``` sh
ctest --test-dir build
```

To create a coverage report, use `lcov`
``` sh
# Install lcov if you haven't already
sudo apt update && sudo apt install lcov

lcov -c -d . -o <path>
```

### Intigrate:

This repo is designed to easily intigrate into other CMake projects.

To link this into your project:
``` CMake
add_subdirectory(<CppUtil path>)
include_directories(src)
```

To link any CppUtil-target to your target:
``` CMake
target_link_libraries
(
  <yourTarget> 
    PUBLIC
      <CppUtil target>
      ...
)
```

## Actions

The Github-Actions-Pipeline tests the following things:
- Build for Linux (ubuntu-latest), Windows (windows-latest) and MacOS (macos-latest) in Debug and Release mode
- Unit tests for all platforms and build modes above
- Code formatting
- Code-Coverage regression

A pull request will only be approved if the pipleline is successfull.
Changes to `.clang-format`  or `ci.yaml` will be checked and approved by hand.