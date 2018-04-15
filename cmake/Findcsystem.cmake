# - Try to find LibXml2
# Once done this will define
#  LIBXML2_FOUND - System has LibXml2
#  CSYSTEM_INCLUDE_DIRS - The LibXml2 include directories
#  LIBXML2_LIBRARIES - The libraries needed to use LibXml2
#  LIBXML2_DEFINITIONS - Compiler switches required for using LibXml2

find_package(PkgConfig)
pkg_check_modules(PC_CSYSTEM QUIET csystem)
set(MBEDTLS_DEFINITIONS ${PC_CSYSTEM_CFLAGS_OTHER})

find_path(CSYSTEM_INCLUDE_DIR csystem.h
          HINTS ${PC_CSYSTEM_INCLUDEDIR} ${PC_CSYSTEM_INCLUDE_DIRS}
          PATH_SUFFIXES csystem)

find_library(CSYSTEM_LIBRARY NAMES csystem csystem_crypto
             HINTS ${PC_CSYSTEM_LIBDIR} ${PC_CSYSTEM_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
##find_package_handle_standard_args(CSYSTEM ""
  ##                                CSYSTEM_LIBRARY CSYSTEM_INCLUDE_DIR)

if(NOT CSYSTEM_INCLUDE_DIR)
    option(ENABLE_PROGRAMS "Build mbed TLS programs." OFF)
    option(ENABLE_TESTING "Build mbed TLS tests." OFF)
    option(CS_BUILD_CRYPTO "" on)
    option(CS_BUILD_TERMINAL "" on)
    add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/csystem)
    include_directories(${PROJECT_SOURCE_DIR}/vendor/csystem/include)
    set(CSYSTEM_LIBRARY csystem csystem_crypto csystem_terminal)
    set(CSYSTEM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/mbedtls/include)
    set(CSYSTEM_FOUND on)
    message(STATUS "Using local csystem")
else()
    message(STATUS "Found csystem")
    set(CSYSTEM_FOUND on)
endif(NOT CSYSTEM_INCLUDE_DIR)

mark_as_advanced(CSYSTEM_INCLUDE_DIR CSYSTEM_LIBRARY)

set(CSYSTEM_LIBRARIES ${CSYSTEM_LIBRARY} )
set(CSYSTEM_INCLUDE_DIRS ${CSYSTEM_INCLUDE_DIR} )