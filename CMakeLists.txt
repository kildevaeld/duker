cmake_minimum_required(VERSION 3.0)
project(dukext)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(DUK_BUILD_TEST "" off)
set(DUK_BUILD_BIN "" off)
set(DUK_BUILD_UV "" off)
#option(DUKER_BUILD_HTTP "build http" off)


include_directories(
	vendor/csystem/include
	include vendor/duktape/src
	vendor/uthash/include
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(csystem)
#find_package(zlib)

#include_directories(vendor/c-thread-pool)

#add_subdirectory(src)
add_subdirectory(src2)

if (DUK_BUILD_UV)
	find_package(libuv)
	add_subdirectory(src2/uv)
endif(DUK_BUILD_UV)

add_subdirectory(example)


if(DUK_BUILD_TEST)
	include(CTest)
	add_subdirectory(vendor/unity)
	add_subdirectory(test)
endif(DUK_BUILD_TEST)