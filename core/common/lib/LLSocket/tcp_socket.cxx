#include <tcp_socket.h>

TCP_Socket_t::TCP_Socket_t(){
    sendBuffer      = new char[TCPBufferSize];
    recieveBuffer   = new char[TCPBufferSize];

    recv_callback = [this](auto socket, std::chrono::nanoseconds rxTime){ 
        std::cout << "[Socket]<" <<
        this << "> Received\n";
    };

}