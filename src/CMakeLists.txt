
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
   
    ${PROJECT_ROOT}/vendor/duktape/src/duktape.c

    ${BUILTINS}
    ${MODULES}

    extras/crypto.c
    extras/zlib.c
    extras/gzip.c

    extras/http.c

    ${PROJECT_ROOT}/vendor/c-thread-pool/thpool.c
    pool.c

)

add_library(duker ${SOURCES})
target_link_libraries(duker
    csystem 
    mbedcrypto
    pthread

    zlibstatic

    libcurl
)
target_include_directories(duker PUBLIC ${PROJECT_ROOT}/include ${PROJECT_ROOT}/vendor/duktape/src)

target_compile_definitions(duker PRIVATE LOG_USE_COLOR)
