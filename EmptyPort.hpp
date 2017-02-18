#pragma once
/* platform-specific headers */

#if defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#define SOCKET_TYPE SOCKET
#define CLOSE_SOCKET(s) closesocket(s)
#pragma comment(lib, "Ws2_32.lib")
#else
#define SOCKET_TYPE int
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#define CLOSE_SOCKET(s) close(s)
#endif

#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <random>
#include <chrono>
#include <thread>

namespace empty_port {

enum Kind { TCP, UDP };

using socket_t = SOCKET_TYPE;
using port_t = uint16_t;
using duration_t = std::chrono::milliseconds;
using clock_t = std::chrono::high_resolution_clock;
static constexpr duration_t SLEEP_DELAY(10);

static constexpr port_t MIN_PORT = 49152;
static constexpr port_t MAX_PORT = 65535;

template <Kind> struct impl;

template <> struct impl<TCP> {

    static bool check_port_impl(const port_t port, const char *host);

    static port_t get_random_impl(const char *host);

    static bool wait_port_impl(const port_t port, const char *host,
                               duration_t max_wait_ms);
};

/* TCP impl */
inline bool impl<TCP>::wait_port_impl(const port_t port, const char *host,
                                      duration_t max_wait_ms) {
    auto stop_at = clock_t::now() + max_wait_ms;
    do {
        if (!impl<TCP>::check_port_impl(port, host)) {
            return true;
        }
        std::this_thread::sleep_for(SLEEP_DELAY);
    } while (clock_t::now() < stop_at);

    return false;
}

inline port_t impl<TCP>::get_random_impl(const char *host) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<port_t> uni(MIN_PORT, MAX_PORT);

    port_t some_port = uni(rng);
    while (some_port < MAX_PORT) {
        if (impl<TCP>::check_port_impl(some_port, host)) {
            return some_port;
        }
        some_port++;
    }
    throw std::runtime_error("Cannot get random port");
}

inline bool impl<TCP>::check_port_impl(const port_t port, const char *host) {
    socket_t s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        throw std::runtime_error("Cannot get random port:: socket");
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

    struct sockaddr_in *host_addr;

    // looks up IPv4/IPv6 address by host name or stringized IP address
    if (getaddrinfo(host, NULL, &hints, &result)) {
        throw std::runtime_error("Cannot get random port:: getaddrinfo");
        goto FAIL;
    }
    host_addr = (struct sockaddr_in *)result->ai_addr;
    memcpy(&remote_addr.sin_addr, &host_addr->sin_addr, sizeof(struct in_addr));
    freeaddrinfo(result);

    if ((connect(s, (struct sockaddr *)&remote_addr, sizeof(remote_addr)))) {
        CLOSE_SOCKET(s);
        return true;
    } else {
        goto FAIL;
    }

FAIL:
    CLOSE_SOCKET(s);
    return false;
}

/* public interface */
template <Kind T>
inline bool check_port(const port_t port, const char *host = "127.0.0.1") {
    return impl<T>::check_port_impl(port, host);
}

template <Kind T> inline port_t get_random(const char *host = "127.0.0.1") {
    return impl<T>::get_random_impl(host);
}

template <Kind T>
inline bool wait_port(const port_t port, const char *host = "127.0.0.1",
                      duration_t max_wait_ms = duration_t(500)) {
    return impl<T>::wait_port_impl(port, host, max_wait_ms);
}
};
