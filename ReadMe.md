[![CI](https://github.com/Hphone123/CppUtil/actions/workflows/ci.yaml/badge.svg)](https://github.com/Hphone123/CppUtil/actions/workflows/ci.yaml)
[![Code Coverage](https://hphone123.github.io/CppUtil/badge.svg)](https://github.com/Hphone123/CppUtil/)

# CppUtil

A hobby project implementing some utility functions and classes in c++.

## Note

**THIS IS A HOBBY PROJECT! I AM NOT AIMING TO IMPROVE ON ANYTHING BESIDES MY OWN CODING SKILL! ANYONE MAY CRITIZE THIS REPOSITORY, BUT PLEASE DO IT IN A FRIENDLY MATTER! IM JUST TRYIN TO LEARN AND ENJOY CODING C++!**

## Prerequesits

All dependencies are installed automaticly via [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started).

## Build

Build using CMAKE:

``` sh
cmake --fresh [flags] -S .
cmake --build .
```



### Catch2

This repo uses [Catch2](https://github.com/catchorg/Catch2) for unit testing. \
Test cases are automaticly run for every target after a successfull build. \
To disable this, set `-DDISABLE_AUTO_TESTING=ON`.

### PCH

All header files are pre-compiled into a pre-compiler header (PCH) to improve compile times.
To disable this, set `-DCREATE_PCH=OFF`.

### Build flags

- `-DALLOW_TEST_FAIL=[ON|OFF]`: Wether to continue the build if a test fails (`OFF` by default)
- `-DRUN_TESTS_AFTER_BUILD=[ON|OFF]`: Wether to run tests after building a target (`ON` by default)
- `-DCREATE_PCH=[ON|OFF]`: Wether to create a PCH for all headers (`ON` by default)

## Intigration

### Using CMAKE

Add this to your CMakeLists.txt: 
``` CMake
add_subdirectory(<path>)
include_directories(src)

...

target_link_libraries
(
  <yourTarget> PUBLIC
    <whatever>
    <you>
    <may>
    <need>
)
```

CMake targets:
- Array (provides `Array`, `ResizableArray`, `DynamicArray`)
- String (provides `String`)
- Exception (provides `not_found`)
- Map (provides `Map`)

### not using CMAKE

All libs are single-header and header-only; you may put any header into your project, just  make sure to also put any header included.

## Files and Classes
- Array.hpp
  - Includes:
    - Exception.hpp
  - Provides:
    - `Array<T>`
    - `ResizableArray<T>`
    - `DynamicArray<T>`
- String.hpp
  - Includes:
    - Array.hpp
    - Exception.hpp
  - Provides:
    - `String`
- Map.hpp
  - Includes:
    - Array.hpp
    - Exception.hpp
  - Provides:
    - `Map<T, U>`
- Exception.hpp
  - Provides:
    - `not_found`
