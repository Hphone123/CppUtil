cmake_minimum_required(VERSION 3.10.0)
project(Fixed VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT MSVC)
	add_compile_options(-Wall)
	add_compile_options(-Werror)
	add_compile_options(-pedantic)

	if (CHECK_COVERAGE)
		add_compile_options(-fkeep-inline-functions) 
		add_compile_options(-fkeep-static-functions)
	endif()
endif(NOT MSVC)

if (BUILD_TESTS)
	find_package(Catch2 REQUIRED)
endif()

add_library(Fixed 
	INTERFACE 
		src/Fixed.hpp)

target_link_libraries(Fixed
	INTERFACE
		Exception)

set_target_properties(Fixed 
	PROPERTIES 
		LINKER_LANGUAGE CXX)

target_include_directories(Fixed 
	INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR}/src)

if (CREATE_PCH)
	target_precompile_headers(Fixed INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/Fixed.hpp)
endif()

if (BUILD_TESTS)
	add_executable(FixedTest 					test/FixedTest.cpp)

	target_link_libraries(FixedTest          PUBLIC Fixed)

	target_link_libraries(FixedTest 				 PRIVATE Catch2::Catch2WithMain)
	
	target_link_libraries(FixedTest  				 PRIVATE CatchVer)

	include(CTest)
	include(Catch)

	catch_discover_tests(FixedTest)
endif()