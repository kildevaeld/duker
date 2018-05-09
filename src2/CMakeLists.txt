

file(GLOB MODULES
    "modules/*.c"
)


set(SOURCES
    ${MODULES}
    dukext.c
    module.c
    sandbox.c
    utils.c
    commonjs.c
    commonjs_file.c
    commonjs_module.c
    console.c
    types.c
    private.c
    ${PROJECT_SOURCE_DIR}/vendor/duktape/src/duktape.c

    zip.c
)

add_library(dukext STATIC ${SOURCES})
target_link_libraries(dukext
    csystem
    csystem_terminal
    #${LIBMBEDTLS_LIBRARIES}
    #pthread
    #${LIBZLIB_LIBRARIES}
)


target_include_directories(dukext PUBLIC 
    ${PROJECT_SOURCE_DIR}/include 
    ${PROJECT_SOURCE_DIR}/vendor/duktape/src
)


target_compile_definitions(dukext PRIVATE LOG_USE_COLOR)
