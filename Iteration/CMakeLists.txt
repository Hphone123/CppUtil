cmake_minimum_required(VERSION 3.10.0)
project(Iteration VERSION 0.1.0)

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

add_library(Iteration 
	INTERFACE 
		src/Iteration.hpp)

set_target_properties(Iteration 
	PROPERTIES 
		LINKER_LANGUAGE CXX)

target_include_directories(Iteration 
	INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR}/src)

if (CREATE_PCH)
	target_precompile_headers(Iteration INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/Iteration.hpp)
endif()

if (BUILD_TESTS)
	add_executable(IterationTest 					test/IterationTest.cpp)
	target_link_libraries(IterationTest          PUBLIC Iteration)
	target_link_libraries(IterationTest 				 PRIVATE Catch2::Catch2WithMain)
	target_link_libraries(IterationTest  				 PRIVATE CatchVer)

	include(CTest)
	include(Catch)

	catch_discover_tests(IterationTest)
endif()