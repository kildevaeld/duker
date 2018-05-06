
add_executable(dukere main.c)
target_link_libraries(dukere dukext dukext_uv dukext-curl dukext-io)

add_library(plugin SHARED lib.c)
target_link_libraries(plugin dukext)