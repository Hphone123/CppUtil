cmake_minimum_required(VERSION 3.10.0)
project(Async VERSION 0.1.0)

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

find_package(Threads REQUIRED)

add_library(Async 
	INTERFACE 
		src/Async.hpp)

target_link_libraries(Async
	INTERFACE
		Threads::Threads
)

set_target_properties(Async 
	PROPERTIES 
		LINKER_LANGUAGE CXX)

target_include_directories(Async 
	INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR}/src)

if (CREATE_PCH)
	target_precompile_headers(Async INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/Async.hpp)
endif()

if (BUILD_TESTS)
	add_executable				(AsyncTest					test/AsyncTest.cpp)

	target_link_libraries	(AsyncTest	PUBLIC	Async)

	target_link_libraries	(AsyncTest	PRIVATE Catch2::Catch2WithMain)
	target_link_libraries (AsyncTest  PRIVATE CatchVer)

	include(CTest)
	include(Catch)

	catch_discover_tests(AsyncTest)
endif()