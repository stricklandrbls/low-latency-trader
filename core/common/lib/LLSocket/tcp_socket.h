#pragma once
#include <functional>
#include <socket_utils.h>
#include <chrono>
#include <iostream>

constexpr size_t TCPBufferSize = 64 * 1024 * 1024;

struct TCP_Socket_t {
    explicit TCP_Socket_t();

    char *sendBuffer    { nullptr };
    char *recieveBuffer { nullptr };
    int fd { -1 };
    size_t nextSendValidIndex   { 0 };
    size_t nextRcvValidIndex    { 0 };
    bool sendDisconnected { false };
    bool recvDisconnected { false };

    struct sockaddr_in inInAddr;
    
    std::function<void(TCP_Socket_t *s, std::chrono::nanoseconds rxTime)> recv_callback;
    std::string timeStr;
};