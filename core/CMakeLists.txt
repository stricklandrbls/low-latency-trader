
add_executable(
    trader-exe
    ./exe/main.cxx
)
add_subdirectory(
    ${COMMON_LIB_DIR}/LLSocket
)
target_link_libraries(
    trader-exe
    LLSocket
)
target_include_directories(
    trader-exe
    PUBLIC ${COMMON_LIB_DIR}
)