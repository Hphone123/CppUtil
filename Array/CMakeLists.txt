cmake_minimum_required(VERSION 3.10.0)
project(Array VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 14)
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

set_target_properties(Array 
	PROPERTIES 
		LINKER_LANGUAGE CXX)

target_include_directories(Array 
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(ArrayTest 
	test/ArrayTest.cpp)
target_link_libraries(ArrayTest PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(ArrayTest)