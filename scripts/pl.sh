#!/bin/bash

if [ -f README.md ]; then
  echo "Could not find README.md, please run this script from the project root"
  exit 1
fi

if [ ! -f "$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" ]; then
  echo "Could not find vcpkg toolchain file!"
  echo "Make sure you have vcpkg set up correctly!"
  exit 1
fi

if [ ! -d build ]; then
  echo "No previous build files found, starting fresh..."
  rm -rf CMakeCache.txt CMakeFiles
  cmake --fresh -S . -B build -G Ninja -DRUN_TESTS_AFTER_BUILD=OFF -DCHECK_COVERAGE=ON -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
fi
cmake --build build --config Debug
ctest --test-dir build
mkdir -p coverage
lcov --capture --directory . --output-file coverage/lcov.info
lcov --remove coverage/lcov.info -o coverage/lcov.info '/usr/**/*' '**/build/**/*' '*Test.cpp'
lcov --list coverage/lcov.info

exit 0