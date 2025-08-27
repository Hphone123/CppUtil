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

if (BUILD_TESTS)
	find_package(Catch2 REQUIRED)
endif()

add_library(String 
	INTERFACE 
		src/String.hpp)

target_link_libraries(String
	INTERFACE 
		Array
		Exception)

set_target_properties(String
	PROPERTIES 
		LINKER_LANGUAGE CXX)

target_include_directories(String
	INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR}/src)

if (CREATE_PCH)
	target_precompile_headers(String INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/String.hpp)
endif()

if (BUILD_TESTS)
	add_executable(StringTest 
		test/StringTest.cpp)

	target_link_libraries(StringTest
		PUBLIC
			String)

	target_link_libraries(StringTest 
		PRIVATE 
			Catch2::Catch2WithMain)
	target_link_libraries (StringTest  
		PRIVATE 
			CatchVer)

	include(CTest)
	include(Catch)
	catch_discover_tests(StringTest)
endif()