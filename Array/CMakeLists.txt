cmake_minimum_required(VERSION 3.10.0)
project(Array VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT MSVC)
	add_compile_options(-Wall)
	add_compile_options(-Werror)
	add_compile_options(-pedantic)
endif(NOT MSVC)

find_package(Catch2 REQUIRED)

add_library(Array 
	STATIC 
		src/Array.hpp)

target_link_libraries(Array
	PUBLIC
		Exception)

set_target_properties(Array 
	PROPERTIES 
		LINKER_LANGUAGE CXX)

target_include_directories(Array 
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/src)

target_precompile_headers(Array INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/Array.hpp)

add_executable(ArrayTest 					test/ArrayTest.cpp)
add_executable(ResizableArrayTest test/ResizableArrayTest.cpp)
add_executable(DynamicArrayTest   test/DynamicArrayTest.cpp)

target_link_libraries(ArrayTest          PUBLIC Array)
target_link_libraries(ResizableArrayTest PUBLIC Array)
target_link_libraries(DynamicArrayTest   PUBLIC Array)

target_link_libraries(ArrayTest 				 PRIVATE Catch2::Catch2WithMain)
target_link_libraries(ResizableArrayTest PRIVATE Catch2::Catch2WithMain)
target_link_libraries(DynamicArrayTest   PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(ArrayTest)
catch_discover_tests(ResizableArrayTest)
catch_discover_tests(DynamicArrayTest)