add_library(
    LLSocket
    SHARED
    ./socket_utils.cxx
    ./tcp_socket.cxx
)
target_include_directories(
    LLSocket
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)