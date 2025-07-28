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

add_subdirectory(String)

add_subdirectory(Map)

add_subdirectory(Exception)

add_custom_target(run_after_build ALL
	COMMENT 
"Running Test cases for Modules:
	- Array
	- String
	- Map"

  DEPENDS ArrayTest
  COMMAND ./Array/ArrayTest

	DEPENDS StringTest
	COMMAND ./String/StringTest

  DEPENDS MapTest
  COMMAND ./Map/MapTest
)
