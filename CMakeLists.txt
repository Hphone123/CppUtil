cmake_minimum_required(VERSION 3.10.0)
project(CppUtil VERSION 0.1.0)

option(ALLOW_TEST_FAIL "Allow tests to fail, but still continue the build" OFF)
option(RUN_TESTS_AFTER_BUILD "Run test cases after successfull build" ON)
option(CREATE_PCH "Create PCH's for all headers" ON)
option(BUILD_TESTS "Create test executables" ON)

set (CMAKE_BUILD_TYPE Debug)

if(ALLOW_TEST_FAIL)
  set(TEST_FAILSAFE || echo Continue...\n)
endif()

if (NOT BUILD_TESTS)
	set(RUN_TESTS_AFTER_BUILD OFF)
else()
	enable_testing()
endif()

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT MSVC)
	add_compile_options(-Wall)
	add_compile_options(-Werror)
	add_compile_options(-pedantic)
endif(NOT MSVC)

add_subdirectory(Array)
if (RUN_TESTS_AFTER_BUILD)
	add_custom_target(RunArrayTest 					ALL COMMENT "Running tests for 'Array'" 					DEPENDS ArrayTest 					COMMAND ./Array/ArrayTest ${TEST_FAILSAFE})
	add_custom_target(RunResizableArrayTest ALL COMMENT "Running tests for 'ResizableArray'"  DEPENDS ResizableArrayTest 	COMMAND ./Array/ResizableArrayTest ${TEST_FAILSAFE})
	add_custom_target(RunDynamicArrayTest 	ALL COMMENT "Running tests for 'DynamicArray'"    DEPENDS DynamicArrayTest    COMMAND ./Array/DynamicArrayTest ${TEST_FAILSAFE})
endif()

add_subdirectory(String)
if (RUN_TESTS_AFTER_BUILD)
	add_custom_target(RunStringTest 				ALL COMMENT "Running tests for 'String'"					DEPENDS StringTest					COMMAND ./String/StringTest ${TEST_FAILSAFE})
endif()

add_subdirectory(Map)
if (RUN_TESTS_AFTER_BUILD)
	add_custom_target(RunMapTest 						ALL COMMENT "Running tests for 'Map'"							DEPENDS MapTest							COMMAND ./Map/MapTest ${TEST_FAILSAFE})
endif()

add_subdirectory(Async)
if (RUN_TESTS_AFTER_BUILD)
	add_custom_target(RunAsyncTest					ALL COMMENT "Running tests for 'Async'"							DEPENDS AsyncTest						COMMAND ./Async/AsyncTest ${TEST_FAILSAFE})
endif()

add_subdirectory(Exception)
add_subdirectory(CatchVer)

