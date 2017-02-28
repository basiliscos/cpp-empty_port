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
