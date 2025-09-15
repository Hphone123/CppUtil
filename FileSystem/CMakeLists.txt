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

add_library(FileSystem 
	STATIC 
		src/FileSystem.hpp
		src/FileSystem.cpp)

target_link_libraries(FileSystem
	PUBLIC 
		Async
		Array
		String
		Platform
)

set_target_properties(FileSystem
	PROPERTIES 
		LINKER_LANGUAGE CXX)

target_include_directories(FileSystem
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/src)

if (CREATE_PCH)
	target_precompile_headers(FileSystem INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/FileSystem.hpp)
endif()

if (BUILD_TESTS)
	add_executable(FileSystemTest 
		test/FileSystemTest.cpp)

	target_link_libraries(FileSystemTest
		PUBLIC
			FileSystem)
	
	target_link_libraries	(FileSystemTest	
		PRIVATE 
			Catch2::Catch2WithMain)

	target_link_libraries (FileSystemTest  
		PRIVATE 
			CatchVer)

	include(CTest)
	include(Catch)
	catch_discover_tests(FileSystemTest)
endif()

add_executable(FileSystemTest_noCatch 
	test/FileSystemTest_noCatch.cpp)

target_link_libraries(FileSystemTest_noCatch
	PUBLIC
		FileSystem)