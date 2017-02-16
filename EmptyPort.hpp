#pragma once
/* platform-specific headers */

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define CLOSE_SOCKET(s) closesocket(s)
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
    #include <unistd.h> /* Needed for close() */
    #define CLOSE_SOCKET(s) close(s)
#endif

#include <cstring>
#include <cstdlib>
#include <stdexcept>

namespace empty_port {

    enum Kind {
        TCP,
        UDP
    };

    using socket_t = int;
    using port_t = uint16_t;

    static constexpr port_t MIN_PORT = 49152;
    static constexpr port_t MAX_PORT = 65535;

    template<Kind>
    struct impl;

    template<>
    struct impl<TCP>{

        static bool check_port_impl(const port_t port, const char* host) {
            socket_t s = socket(AF_INET, SOCK_STREAM, 0);
            if (s == -1) {
                return false;
            }
            sockaddr_in remote_addr;
            std::memset(&remote_addr, 0, sizeof(remote_addr));
            remote_addr.sin_family = AF_INET;
            remote_addr.sin_port = htons(port);

            struct addrinfo hints, *result = NULL;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            struct sockaddr_in* host_addr;

            // looks up IPv4/IPv6 address by host name or stringized IP address
            if ( getaddrinfo(host, NULL, &hints, &result) ) {
                goto FAIL;
            }
            host_addr = (struct sockaddr_in*)result->ai_addr;
            memcpy(&remote_addr.sin_addr, &host_addr->sin_addr, sizeof(struct in_addr));
            freeaddrinfo(result);

            if((connect(s, (struct sockaddr *)&remote_addr, sizeof(remote_addr))) < 0) {
                CLOSE_SOCKET(s);
                return true;
            } else {
                goto FAIL;
            }

            FAIL:
            CLOSE_SOCKET(s);
            return false;
        }

        static port_t get_random_impl(const char* host) {
            static constexpr port_t delta = (MAX_PORT - MIN_PORT) >> 2;

            port_t some_port = MIN_PORT + (int(std::rand() * delta)) % delta;
            while (some_port < MAX_PORT) {
                if (check_port_impl(some_port, host)) {
                    return some_port;
                }
                some_port++;
            }
            throw std::runtime_error("Cannot get random port");
        }

    };

    template<Kind T>
    bool check_port(const port_t port, const char* host = "127.0.0.1") {
        return impl<T>::check_port_impl(port, host);
    }

    template<Kind T>
    port_t get_random(const char* host = "127.0.0.1") {
        return impl<T>::get_random_impl(host);
    }

};