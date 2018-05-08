
set(SOURCES
   io.c
   reader.c
   writer.c
   file.c
)

add_library(dukext-io ${SOURCES})
target_link_libraries(dukext-io dukext)