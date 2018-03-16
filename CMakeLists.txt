cmake_minimum_required(VERSION 3.0)
project(duker)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(DUK_BUILD_TEST "" off)
set(DUK_BUILD_BIN "" off)


option(DUKER_BUILD_HTTP "build http" off)


#add_subdirectory(vendor/csystem)

include_directories(
	vendor/csystem/include
	include vendor/duktape/src
	vendor/uthash/include
)


##include_directories(vendor/boringssl/include)
##add_subdirectory(vendor/boringssl)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(csystem)
find_package(libuv)
#find_package(mbedtls)
find_package(zlib)


## Curl
# if (DUKER_BUILD_HTTP)
# 	option(HTTP_ONLY "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)" ON)
# 	set(MBEDTLS_INCLUDE_DIRS vendor/mbedtls/include)
# 	link_directories(${CMAKE_CURRENT_BINARY_DIR}/vendor/mbedtls/library)
# 	include_directories(vendor/curl/include)
# 	set(MBEDTLS_LIBRARY mbedtls)
# 	set(MBEDCRYPTO_LIBRARY mbedcrypto)
# 	set(MBEDX509_LIBRARY mbedx509)
# 	option(BUILD_CURL_EXE "Set to ON to build curl executable." OFF)
# 	option(CMAKE_USE_MBEDTLS "Enable mbedTLS for SSL/TLS" ON)
# 	option(CURL_STATICLIB "Set to ON to build libcurl with static linking." ON)
# 	set(BUILD_TESTING OFF CACHE BOOL "test")
# 	add_subdirectory(vendor/curl)
# endif(DUKER_BUILD_HTTP)
## Worker Pool

include_directories(vendor/c-thread-pool)



add_subdirectory(src)

##add_subdirectory(vendor/uvhttp)
##include_directories(vendor/uvhttp/include)
#include_directories(vendor/uvhttp/vendor/libuv/include)
add_subdirectory(src/uv)


add_subdirectory(example)


if(DUKER_BUILD_TEST)
	include(CTest)
	add_subdirectory(vendor/unity)
	add_subdirectory(test)
endif(DUKER_BUILD_TEST)