# - Try to find LibXml2
# Once done this will define
#  LIBXML2_FOUND - System has LibXml2
#  LIBZLIB_INCLUDE_DIRS - The LibXml2 include directories
#  LIBXML2_LIBRARIES - The libraries needed to use LibXml2
#  LIBXML2_DEFINITIONS - Compiler switches required for using LibXml2

find_package(PkgConfig)
pkg_check_modules(PC_ZLIB QUIET zlib)
set(LIBZLIB_DEFINITIONS ${PC_ZLIB_CFLAGS_OTHER})

find_path(LIBZLIB_INCLUDE_DIR inflate.h defalte.h
          HINTS ${PC_ZLIB_INCLUDEDIR} ${PC_ZLIB_INCLUDE_DIRS}
          PATH_SUFFIXES mbedtls )

find_library(LIBZLIB_LIBRARY NAMES z
             HINTS ${PC_ZLIB_LIBDIR} ${PC_ZLIB_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
##find_package_handle_standard_args(libmbedtls ""
  ##                                LIBZLIB_LIBRARY LIBZLIB_INCLUDE_DIR)

if(NOT LIBZLIB_LIBRARY)
    option(ENABLE_PROGRAMS "Build mbed TLS programs." OFF)
    option(ENABLE_TESTING "Build mbed TLS tests." OFF)
    add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/zlib)
    include_directories(${PROJECT_SOURCE_DIR}/vendor/zlib)
    set(LIBZLIB_LIBRARY zlibstatic)
    set(LIBZLIB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/zlib)
    set(LIBZLIB_FOUND on)
    message(STATUS "Using local zlib")
else()
    message(STATUS "Found zlib")
    set(LIBZLIB_FOUND on)
endif(NOT LIBZLIB_LIBRARY) 

mark_as_advanced(LIBZLIB_INCLUDE_DIR LIBZLIB_LIBRARY)

set(LIBZLIB_LIBRARIES ${LIBZLIB_LIBRARY} )
set(LIBZLIB_INCLUDE_DIRS ${LIBZLIB_INCLUDE_DIR})