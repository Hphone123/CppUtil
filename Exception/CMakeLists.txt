cmake_minimum_required(VERSION 3.10.0)
project(Exception VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT MSVC)
	add_compile_options(-Wall)
	add_compile_options(-Werror)
	add_compile_options(-pedantic)
endif(NOT MSVC)

find_package(Catch2 REQUIRED)

add_library(Exception 
	STATIC 
		src/Exception.hpp)
		
set_target_properties(Exception 
	PROPERTIES 
		LINKER_LANGUAGE CXX)
		
target_precompile_headers(Exception INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/Exception.hpp)

target_include_directories(Exception 
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/src)