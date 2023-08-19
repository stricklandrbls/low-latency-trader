#pragma once
#include <iostream>
#include <string>
#include <unordered_set>

#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <ifaddrs.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>


namespace Socket {

constexpr int MaxTCPServerBacklog = 1024;

auto get_interface_IP(const std::string &iface) -> std::string;

/**
 * @brief Sets the socket assigned to the file descriptor to be non-blocking.
 * 
 * A blocking socket is one where a call that is read on it will block indefinitely till data
 * is available. This is a high resource feature because blocking uses switches between user and 
 * kernal space, due to the requirement for interrupts.
*/
auto set_non_blocking(int fd) -> bool;

/**
 * @brief Disables Nagle's algorithm which improves buffering in TCP sockets to prevent overhead
 * associated with guaranteeing reliability on the TCP socket. For many applications, this is a 
 * good feature to have.
*/
auto set_no_delay(int fd) -> bool;

/**
 * @brief Create timestamp for packets hitting the network socket
*/
auto set_SO_timestamp(int fd) -> bool;

/** 
 * @brief Sets the Time to Live(TTL) value. TTL is a network-level setting that controls the max
 * number of hops that a packet can take from sender to receiver.
*/
auto set_ttl(int fd, int ttl) -> bool;
/** 
 * @brief Sets the Time to Live(TTL) value. TTL is a network-level setting that controls the max
 * number of hops that a packet can take from sender to receiver.
*/
auto set_multicast_ttl(int fd, int ttl) -> bool;


auto would_block() -> bool;
auto join(int fd, const std::string &ip, const std::string &iface, int port) -> bool;
auto create_socket(const std::string &t_ip, const std::string &iface, int port, bool is_udp,
    bool is_blocking, bool is_listening, int ttl, bool needs_so_timestamp) -> int;
}