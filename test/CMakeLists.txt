
set(SOURCES

    main.c
)

add_executable(duker_test ${SOURCES})
target_link_libraries(duker_test dukext unity)
add_test(duker_test duker_test)
