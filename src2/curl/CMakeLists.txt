
set(SOURCES
    curl.c
    client.c
    #form.c
    #header.c
)

add_library(dukext-curl ${SOURCES})
target_link_libraries(dukext-curl dukext curl)