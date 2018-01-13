
file(GLOB BUILTINS
    "builtins/*.c"
)

file(GLOB MODULES
    "modules/*.c"
)

set(SOURCES
    duker.c
    module.c
    log.c
    schema.c
    utils.c
    ${PROJECT_ROOT}/vendor/duktape/src/duktape.c

    ${BUILTINS}
    ${MODULES}
)

add_library(duker ${SOURCES})
target_link_libraries(duker csystem)

target_compile_definitions(duker PRIVATE LOG_USE_COLOR)
