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

add_library(String 
	STATIC 
		src/String.hpp)

target_link_libraries(String
	PUBLIC 
		Array
		Exception)

set_target_properties(String
	PROPERTIES 
		LINKER_LANGUAGE CXX)

target_include_directories(String
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(StringTest 
	test/StringTest.cpp)

target_link_libraries(StringTest
	PUBLIC
		String)

target_link_libraries(StringTest 
	PRIVATE 
		Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(StringTest)