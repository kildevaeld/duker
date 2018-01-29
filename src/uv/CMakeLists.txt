
set(SOURCES
    uv-module.c
    timers.c
)


add_library(duker_uv ${SOURCES})
target_link_libraries(duker_uv uvhttp uv)