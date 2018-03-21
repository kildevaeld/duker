
add_executable(dukere main.c)
target_link_libraries(dukere duker duker_uv)

add_library(plugin SHARED lib.c)
target_link_libraries(plugin duker)