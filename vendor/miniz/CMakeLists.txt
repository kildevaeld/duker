
set(SOURCES
    miniz-src/miniz.c
    miniz-src/miniz_tdef.c
    miniz-src/miniz_tinfl.c
    miniz-src/miniz_zip.c
)

add_library(miniz ${SOURCES})
target_include_directories(miniz PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/miniz-src)