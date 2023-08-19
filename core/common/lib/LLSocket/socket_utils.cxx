#include <socket_utils.h>

namespace Socket {

/**
 * @brief Query network interface adapters and obtain the IP address if one matches the argument.
 * @return char[]
*/
auto get_interface_IP(const std::string &iface) -> std::string {
    char buf[NI_MAXHOST] {'\0'};
    ifaddrs *ifaddr = nullptr;

    if( getifaddrs(&ifaddr) != -1){
        for(ifaddrs *ifa = ifaddr; ifa; ifa = ifa->ifa_next){
            if(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && iface == ifa->ifa_name){
                getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
                break;
            }
        }
        freeifaddrs(ifaddr);
    }
    return buf;
}

auto set_non_blocking(int fd) -> bool {
    const auto flags = fcntl(fd, F_GETFL, 0);
    if( flags == -1 ) return false;
    if( flags & O_NONBLOCK ) return true;
    return ( fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1 );
}

auto set_no_delay(int fd) -> bool {
    int one = 1;
    return (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<void*>(&one), sizeof(one)) != -1);
}

auto would_block() -> bool {
    return (errno == EWOULDBLOCK || errno == EINPROGRESS);
}

auto set_ttl(int fd, int ttl) -> bool {
    return (setsockopt(fd, IPPROTO_IP, IP_TTL, reinterpret_cast<void*>(&ttl), sizeof(ttl)) != -1);
}

auto set_multicast_ttl(int fd, int ttl) -> bool {
    return (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, reinterpret_cast<void*>(&ttl), sizeof(ttl)) != -1);
}

auto set_SO_timestamp(int fd) -> bool {
    int one = 1;
    return (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, reinterpret_cast<void*>(&one), sizeof(one)) != -1);
}

auto create_socket(const std::string &t_ip, const std::string &iface, int port, bool is_udp,
    bool is_blocking, bool is_listening, int ttl, bool needs_so_timestamp) -> int {
        is_listening = false;
        is_udp = false;

        std::string time_str;
        const auto ip = t_ip.empty() ? get_interface_IP(iface) : t_ip;

        addrinfo hints;
        hints.ai_family = AF_INET;
        hints.ai_socktype = is_udp ? SOCK_DGRAM : SOCK_STREAM;
        hints.ai_protocol = is_udp ? IPPROTO_UDP : IPPROTO_TCP;
        hints.ai_flags = is_listening ? AI_PASSIVE : 0;

        if(std::isdigit(ip.c_str()[0]))
            hints.ai_flags |= AI_NUMERICHOST;
        hints.ai_flags |= AI_NUMERICSERV;

        addrinfo *result { nullptr };
        const auto rc = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &result);
        if(rc) {
            std::cerr << "[Socket] rc error\n";
            return -1;
        }

        int fd = -1;
        int one = 1;
        for(addrinfo *rp = result; rp; rp = rp->ai_next) {
            fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if(fd == -1) {
                std::cerr << "[Socket] error creating socket\n";
                return -1;
            }

            if(!is_blocking){
                if(!set_non_blocking(fd)) {
                    std::cerr << "[Socket] error setting non-block\n";
                    return -1;
                }
                if(!is_udp && !set_no_delay(fd)) {
                    std::cerr << "[Socket] error setting no-delay\n";
                    return -1;
                }
            }
            if(!is_listening && connect(fd, rp->ai_addr, rp->ai_addrlen) == 1 && !would_block()){
                std::cerr << "[Socket] connect failed\n";
                return -1;
            }
            if(is_listening && setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&one),sizeof(one)) == -1) {
                std::cerr << "[Socket] SO_REUSE failed\n";
                return -1;
            }
            if(is_listening && bind(fd, rp->ai_addr, rp->ai_addrlen) == -1) {
                std::cerr << "[Socket] bind failed\n";
                return -1;
            }
            if(!is_udp && is_listening && listen(fd, MaxTCPServerBacklog) == -1) {
                std::cerr << "[Socket] listen() failed\n";
                return -1;
            }
            if(is_udp && ttl) {
                const bool is_multicast = atoi(ip.c_str()) & 0xe0;
                if(is_multicast && !set_multicast_ttl(fd, ttl)) {
                    std::cerr << "[Socket] setMcastTTL() failed\n";
                    return -1;
                }
                if(!is_multicast && !set_ttl(fd, ttl)) {
                    std::cerr << "[Socket] set_ttl() failed\n";
                    return -1;
                }
            }
            if(needs_so_timestamp && !set_SO_timestamp(fd)) {
                std::cerr << "[Socket] set_so_timestamp() failed\n";
                return -1;
            }
        }
        if(result)
            freeaddrinfo(result);
        return fd;
    }
}