# - Try to find LibXml2
# Once done this will define
#  LIBXML2_FOUND - System has LibXml2
#  LIBUV_INCLUDE_DIRS - The LibXml2 include directories
#  LIBXML2_LIBRARIES - The libraries needed to use LibXml2
#  LIBXML2_DEFINITIONS - Compiler switches required for using LibXml2



find_package(PkgConfig)
pkg_check_modules(PC_LIBUV QUIET libuv)
set(LIBUV_DEFINITIONS ${PC_LIBUV_CFLAGS_OTHER})

find_path(LIBUV_INCLUDE_DIR uv.h
          HINTS ${PC_LIBUV_INCLUDEDIR} ${PC_LIBUV_INCLUDE_DIRS}
          PATH_SUFFIXES uv )

find_library(LIBUV_LIBRARY NAMES uv
             HINTS ${PC_LIBUV_LIBDIR} ${PC_LIBUV_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(libuv  DEFAULT_MSG
                                  LIBUV_LIBRARY LIBUV_INCLUDE_DIR)


mark_as_advanced(LIBUV_INCLUDE_DIR LIBUV_LIBRARY)

set(LIBUV_LIBRARIES ${LIBUV_LIBRARY} )
set(LIBUV_INCLUDE_DIRS ${LIBUV_INCLUDE_DIR} )