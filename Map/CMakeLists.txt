cmake_minimum_required(VERSION 3.10.0)
project(Map VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT MSVC)
	add_compile_options(-Wall)
	add_compile_options(-Werror)
	add_compile_options(-pedantic)
endif(NOT MSVC)

# Find Catch2

find_package(Catch2 REQUIRED)

# Add lib

add_library(Map STATIC 
	src/Map.hpp
	)

# Link Array Header

target_link_libraries(Map 
	PUBLIC 
		Array
		Exception)

# Set Properties

target_include_directories(Map 
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(Map 
	PROPERTIES 
		LINKER_LANGUAGE CXX)

if (CREATE_PCH)
	target_precompile_headers(Map INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/Map.hpp)
endif()


add_executable			 (MapTest 	
	test/MapTest.cpp)

target_link_libraries(MapTest 
	PUBLIC
		Map)

target_link_libraries(MapTest 
	PRIVATE 
		Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(MapTest)