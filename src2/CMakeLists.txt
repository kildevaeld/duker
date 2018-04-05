


set(SOURCES
    dukext.c
    module.c
    sandbox.c
    utils.c
    commonjs.c
    commonjs_file.c
    commonjs_module.c
    console.c
    ${PROJECT_ROOT}/vendor/duktape/src/duktape.c
)

add_library(dukext SHARED ${SOURCES})
target_link_libraries(dukext
    csystem 
    #${LIBMBEDTLS_LIBRARIES}
    pthread
    ${LIBZLIB_LIBRARIES}
)


target_include_directories(dukext PUBLIC ${PROJECT_ROOT}/include ${PROJECT_ROOT}/vendor/duktape/src)

target_compile_definitions(dukext PRIVATE LOG_USE_COLOR)
