cmake_minimum_required(VERSION 3.0)
project(duker)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(DUKER_BUILD_TEST "" off)
set(DUKER_BUILD_BIN "" off)

add_subdirectory(vendor/csystem)

include_directories(
	vendor/csystem/include
	include vendor/duktape/src
	vendor/uthash/include
)


include_directories(vendor/boringssl/include)
add_subdirectory(vendor/boringssl)


add_subdirectory(src)
add_subdirectory(example)

if(DUKER_BUILD_TEST)

	include(CTest)
	add_subdirectory(vendor/unity)
	add_subdirectory(test)

endif(DUKER_BUILD_TEST)