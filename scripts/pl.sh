#!/bin/bash

if [ -f README.md ]; then
  echo "Could not find README.md, please run this script from the project root"
  exit 1
fi

cmake cmake -S . -B build -G Ninja -DRUN_TESTS_AFTER_BUILD=OFF -DCHECK_COVERAGE=ON -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build
ctest --test-dir build
mkdir -p coverage                                                                                                                                                ─╯
lcov --capture --directory . --output-file coverage/lcov.info
lcov --remove coverage/lcov.info -o coverage/lcov.info '/usr/**/*' '**/build/**/*' '*Test.cpp'
lcov --list coverage/lcov.info

exit 0