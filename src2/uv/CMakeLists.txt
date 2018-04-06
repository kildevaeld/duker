
set(SOURCES
    uv.c
    timers.c
    bag.c
    fs.c
)

add_library(dukext_uv ${SOURCES})
target_link_libraries(dukext_uv dukext ${LIBUV_LIBRARIES})

