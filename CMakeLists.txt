cmake_minimum_required(VERSION 3.0)
project(duker)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(vendor/csystem)

include_directories(
	vendor/csystem/include
	include vendor/duktape/src
	vendor/uthash/include
)

add_subdirectory(src)
add_subdirectory(example)