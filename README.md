# cpp-empty_port

Finds empty port 

linux [![Build Status](https://travis-ci.org/basiliscos/cpp-empty_port.png)](https://travis-ci.org/basiliscos/cpp-empty_port.png), windows [![Build status](https://ci.appveyor.com/api/projects/status/7lqgpi6kf8ajyd50?svg=true)](https://ci.appveyor.com/project/basiliscos/cpp-empty-port)

# synopsis

```cpp
#include <EmptyPort.hpp>

namespace ep = empty_port;


TEST_CASE("..." ) {
    /* get first free port */
    auto port = ep::get_random();
    
    /* returns true or false */
    ep::check_port(port);
    
    /* waits until port will be occupied */
    ep::wait_port(port);
}    
```

# API

```cpp

/* retrun true if port is ready for usage */
bool check_port(const port_t port, const char *host = "127.0.0.1");

/* returns first available empty port or throws exception */
uint16_t get_random(const char *host = "127.0.0.1");

/* waits until up to max_wait_ms before porta will be occupied. 
Retruns true if it is, otherwise returns false */
<Kind T = Kind::TCP, typename D = std::chrono::milliseconds>
bool wait_port(const port_t port, const char *host = "127.0.0.1", D max_wait = D(500))
```

