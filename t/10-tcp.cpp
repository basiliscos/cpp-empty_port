#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <EmptyPort.hpp>

namespace ep = empty_port;
constexpr ep::Kind TCP = ep::Kind::TCP;

TEST_CASE( "port 10001 & 80", "[tcp]" ) {
    REQUIRE( ep::check_port<TCP>(10001) );
    REQUIRE( !ep::check_port<TCP>(80) );
}

TEST_CASE( "100 random empty ports", "[tcp]" ) {
    static constexpr size_t COUNT = 100;
    ep::port_t ports[COUNT] = {0};
    for (auto i =0; i < COUNT; i++) {
        ports[i] = ep::get_random<TCP>();
        REQUIRE(ports[i] > 0);
    }
    // check for uniqueness
    for (auto i =0; i < COUNT; i++) {
        for (auto j = i+1; j < COUNT; j++) {
            REQUIRE(ports[i] != ports[j]);
        }
    }
}
