
set(SOURCES
   io.c
   reader.c
   writer.c
)

add_library(dukext-io ${SOURCES})
target_link_libraries(dukext-io dukext)