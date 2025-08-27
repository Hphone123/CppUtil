cmake_minimum_required(VERSION 3.10.0)
project(CatchVer VERSION 1.0.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT MSVC)
	add_compile_options(-Wall)
	add_compile_options(-Werror)
	add_compile_options(-pedantic)
endif(NOT MSVC)

add_library(CatchVer 
	INTERFACE 
		CatchVer.hpp)

set_target_properties(CatchVer 
	PROPERTIES 
		LINKER_LANGUAGE CXX)

target_include_directories(CatchVer 
	INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR})

if (CREATE_PCH)
	target_precompile_headers(CatchVer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CatchVer.hpp)
endif()