cmake_minimum_required(VERSION 3.10.0)
project(CppUtil VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT MSVC)
	add_compile_options(-Wall)
	add_compile_options(-Werror)
	add_compile_options(-pedantic)
endif(NOT MSVC)

add_subdirectory(Array)
	add_custom_target(RunArrayTest 					ALL COMMENT "Running tests for 'Array'" 					DEPENDS ArrayTest 					COMMAND ./Array/ArrayTest)
	add_custom_target(RunResizableArrayTest ALL COMMENT "Running tests for 'ResizableArray'"  DEPENDS ResizableArrayTest 	COMMAND ./Array/ResizableArrayTest)
	add_custom_target(RunDynamicArrayTest 	ALL COMMENT "Running tests for 'DynamicArray'"    DEPENDS DynamicArrayTest    COMMAND ./Array/DynamicArrayTest)

add_subdirectory(String)
	add_custom_target(RunStringTest 				ALL COMMENT "Running tests for 'String'"					DEPENDS StringTest					COMMAND ./Array/StringTest)

add_subdirectory(Map)
	add_custom_target(RunMapTest 						ALL COMMENT "Running tests for 'Map'"							DEPENDS MapTest							COMMAND ./Map/MapTest)

add_subdirectory(Exception)


