# - Try to find LibXml2
# Once done this will define
#  LIBXML2_FOUND - System has LibXml2
#  LIBMBEDTLS_INCLUDE_DIRS - The LibXml2 include directories
#  LIBXML2_LIBRARIES - The libraries needed to use LibXml2
#  LIBXML2_DEFINITIONS - Compiler switches required for using LibXml2

find_package(PkgConfig)
pkg_check_modules(PC_MBEDTLS QUIET mbedtls)
set(MBEDTLS_DEFINITIONS ${PC_MBEDTLS_CFLAGS_OTHER})

find_path(LIBMBEDTLS_INCLUDE_DIR ssl.h md5.h
          HINTS ${PC_MBEDTLS_INCLUDEDIR} ${PC_LIBMBEDTLS_INCLUDE_DIRS}
          PATH_SUFFIXES mbedtls )

find_library(LIBMBEDTLS_LIBRARY NAMES libmbedcrypto libmbedtls
             HINTS ${PC_LIBMBEDTLS_LIBDIR} ${PC_LIBMBEDTLS_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
##find_package_handle_standard_args(libmbedtls ""
  ##                                LIBMBEDTLS_LIBRARY LIBMBEDTLS_INCLUDE_DIR)

if(NOT LIBMBEDTLS_INCLUDE_DIR)
    option(ENABLE_PROGRAMS "Build mbed TLS programs." OFF)
    option(ENABLE_TESTING "Build mbed TLS tests." OFF)
    add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/mbedtls)
    include_directories(${PROJECT_SOURCE_DIR}/vendor/mbedtls/include)
    set(LIBMBEDTLS_LIBRARY mbedcrypto mbedtls)
    set(LIBMBEDTLS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/mbedtls/include)
    set(LIBMBEDTLS_FOUND on)
    message(STATUS "Using local libmbedtls")
else()
    message(STATUS "Found libmbedtls")
    set(LIBMBEDTLS_FOUND on)
endif(NOT LIBMBEDTLS_INCLUDE_DIR)

mark_as_advanced(LIBMBEDTLS_INCLUDE_DIR LIBMBEDTLS_LIBRARY)

set(LIBMBEDTLS_LIBRARIES ${LIBMBEDTLS_LIBRARY} )
set(LIBMBEDTLS_INCLUDE_DIRS ${LIBMBEDTLS_INCLUDE_DIR} )