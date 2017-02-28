# cpp-empty_port

Finds empty port (C++, header-only)

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

# Description

This tiny library mimics Perl's [Net::EmptyPort](https://metacpan.org/pod/Net::EmptyPort). The idea is that it mihgt be rather easy to launch 3rd-party services, instead of mocking them, i.e. launch **redis**

```cpp

namespace ep = empty_port;
namespace ts = test_server;

...;

uint16_t port = ep::get_random<ep::Kind::TCP>();
LOG_DEBUG("Using port :" << port);

auto port_str = boost::lexical_cast<std::string>(port);
auto server = ts::make_server({"redis-server", "--port", port_str});
ep::wait_port<ep::Kind::TCP>(port);


```

or simple http-server, quickly written in scripting language:

```perl
# app.pgsi
my $app = sub {
    my $env = shift;
    return [
        '200',
        [ 'Content-Type' => 'text/plain' ],
        [ "Hello World" ],
    ];
};

```

and then use it in unit-tests:

```cpp

namespace ep = empty_port;
namespace ts = test_server;

...;

uint16_t port = ep::get_random();
LOG_DEBUG("Using port :" << port);

auto port_str = boost::lexical_cast<std::string>(port);
auto server = ts::make_server({"plackup", "--port", port_str, "../t/app.psgi"} );
ep::wait_por(port);
LOG_DEBUG("app-server is ready on port " << port_str);

```

The test-server RAII is unix-specific:

```cpp
#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

namespace test_server {
    struct TestServer {
        pid_t child_pid;
        TestServer(std::initializer_list<std::string>&& args) {
            auto begin = args.begin();
            auto end = args.end();

            auto program = (*begin).c_str();
            auto args_count = args.size();
            const char** c_args = (const char**) std::malloc(sizeof(char*) * (args_count + 1));
            auto it = begin;
            std::string stringized;
            for (auto i = 0; i < args_count; i++, it++) {
                const char* c_arg = (*it).c_str();
                c_args[i] = c_arg;
                stringized += " ";
                stringized += c_arg;
            }
            c_args[args_count] = NULL;


            std::cout << "going to fork to start: " << stringized << "\n";

            pid_t pid = fork();
            if (pid == -1) {
                throw std::runtime_error("cannot fork");
            } else if (pid == 0) {
                // child
                std::cout << "executing in child" << "\n";
                int result = execvp(program, (char* const*)c_args);
                std::cout <<"failed to execute: " << strerror(errno) << "\n";
                exit(-1);
            } else {
                // parent
                child_pid = pid;
                // sleep(5);
            }
        }
        ~TestServer() {
            std::cout << "terminating child " << child_pid << "\n";
            kill(child_pid, 9);
        }
    };


    using result_t = std::unique_ptr<TestServer>;

    result_t make_server(std::initializer_list<std::string>&& args) {
        return std::make_unique<TestServer>(std::move(args));
    }

};
```


