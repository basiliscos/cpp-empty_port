#define CATCH_CONFIG_MAIN

#include <EmptyPort.hpp>
#include "catch.hpp"

namespace ep = empty_port;
constexpr ep::Kind TCP = ep::Kind::TCP;

static bool initialised = false;

static void initialise() {
    if (initialised == false) {
#if defined(_WIN32) || defined(WIN32)
        WSADATA data = {0};
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
#endif
        initialised = true;
    }
}

TEST_CASE("single port", "[tcp]") {
    initialise();
    auto port = ep::get_random<TCP>();
    REQUIRE(port > 0);
    REQUIRE(ep::check_port<TCP>(port));
    REQUIRE(!ep::wait_port<TCP>(port));

    auto s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    listen(s, 5);

    REQUIRE(!ep::check_port<TCP>(port));
    REQUIRE(ep::wait_port<TCP>(port));
}

TEST_CASE("10 random empty ports", "[tcp]") {
    static constexpr size_t COUNT = 10;
    ep::port_t ports[COUNT] = {0};
    for (auto i = 0; i < COUNT; i++) {
        ports[i] = ep::get_random<TCP>();
        REQUIRE(ports[i] > 0);
    }
    // check for uniqueness
    for (auto i = 0; i < COUNT; i++) {
        for (auto j = i + 1; j < COUNT; j++) {
            REQUIRE(ports[i] != ports[j]);
        }
    }
}
