
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
    refs.c
    #utils.c
    helpers.c
    duker_console.c
   
    ${PROJECT_ROOT}/vendor/duktape/src/duktape.c

    ${BUILTINS}
    ${MODULES}

    #extras/crypto.c
    extras/zlib.c
    #extras/gzip.c

    #extras/http.c

    #${PROJECT_ROOT}/vendor/c-thread-pool/thpool.c
    #pool.c

    # Modules
    event-emitter.c

)

add_library(duker SHARED ${SOURCES})
target_link_libraries(duker
    csystem 
    #${LIBMBEDTLS_LIBRARIES}
    pthread
    ${LIBZLIB_LIBRARIES}
)


if(DUKER_BUILD_HTTP)
    target_link_libraries(duker libcurl)
endif(DUKER_BUILD_HTTP)

target_include_directories(duker PUBLIC ${PROJECT_ROOT}/include ${PROJECT_ROOT}/vendor/duktape/src)

target_compile_definitions(duker PRIVATE LOG_USE_COLOR)

if(DUKER_BUILD_HTTP)
    target_compile_definitions(duker PRIVATE DUKER_USE_HTTP)
endif(DUKER_BUILD_HTTP)