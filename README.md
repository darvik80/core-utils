# core-utils

|Library|fmt (std:fmt in cpp20)|
|---|---|
|link|https://github.com/fmtlib/fmt|
|descr|{fmt} is an open-source formatting library providing a fast and safe alternative to C stdio and C++ iostreams.|

## Logging

#### Logging.h

```cpp
#pragma once

#include <logging/Logging.h>

LOG_COMPONENT_SETUP(em, em_logger);
```

#### main.cpp

```cpp
#include "Logging"

int main(int argc, char *argv[]) {
    logger::LoggingProperties logProps;
    logProps.level="info";
    logger::setup(logProps);


    em::log::info("em::info");
    em::log::warning("em::warn");

    return 0;
}
```

![](https://raw.githubusercontent.com/darvik80/core-utils/master/images/logging.png)

## Event Manager

#### main.cpp

```cpp
#include "event/EventManagerLogger.h"
#include "scheduler/Scheduler.h"
#include "event/EventManager.h"

int main(int argc, char *argv[]) {
    boost::asio::io_service service;

    Scheduler scheduler(service);

    scheduler.scheduleAtFixedRate([]() {
        em::log::info("scheduleAtFixedRate");
    }, boost::posix_time::seconds{0}, boost::posix_time::seconds{10});

    scheduler.scheduleWithFixedDelay([]() {
        em::log::info("scheduleWithFixedDelay");
    }, boost::posix_time::seconds{0}, boost::posix_time::seconds{5});

    scheduler.schedule([]() {
        em::log::info("schedule");
    }, boost::posix_time::seconds{10});

    em::EventManager mng;
    mng.subscribe<em::Event>([](const em::Event& event) -> bool {
        em::log::info("handle event");
        return false;
    });

    mng.raiseEvent(em::Event{});

    service.run();
    return 0;
}
```

![](https://raw.githubusercontent.com/darvik80/core-utils/master/images/event-manager.png)

## Network

#### main.cpp

```cpp
#include <vector>
#include "network/Handler.h"
#include "network/handler/NetworkLogger.h"
#include "network/boost/AsyncTcpServer.h"
#include "network/boost/AsyncTcpClient.h"
#include "network/zeromq/ZeroMQCodec.h"
#include "network/zeromq/ZeroMQHandler.h"

using namespace network;

#include <logging/Logging.h>

LOG_COMPONENT_SETUP(app, app_logger);

int main(int argc, char *argv[]) {
    logger::LoggingProperties logProps;
    logProps.level = "debug";
    logger::setup(logProps);

    boost::asio::io_service service;

    boost::asio::signal_set signals(service);
    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif

    uint16_t port = 5556;

    auto subscriber = std::make_shared<zeromq::ZeroMQSubscriber>();
    subscriber->subscribe("joystick", [](std::string_view topic, std::string_view data) {
        logger::info("sub: {}:{}", topic, data);
    });

//    AsyncServer<SslSocket> server(
//            service,
//            [subscriber](const std::shared_ptr<AsyncChannel<SslSocket>> &channel) {
//                link(
//                        channel,
//                        std::make_shared<handler::NetworkLogger>(),
//                        std::make_shared<zeromq::ZeroMQCodec>(),
//                        subscriber
//                );
//            },
//            "/Users/darvik/server.pem",
//            "/Users/darvik/server_key.pem"
//    );
    AsyncServer<TcpSocket> server(
            service,
            [subscriber](const std::shared_ptr<AsyncChannel<TcpSocket>> &channel) {
                link(
                        channel,
                        std::make_shared<handler::NetworkLogger>(),
                        std::make_shared<zeromq::ZeroMQCodec>(),
                        subscriber
                );
            }
    );

    server.bind(port);

    auto producer = std::make_shared<zeromq::CompositeProducer>();
//    AsyncClient<SslSocket> client(
//            service,
//            [producer](const std::shared_ptr<AsyncChannel<SslSocket>> &channel) {
//                link(
//                        channel,
//                        std::make_shared<handler::NetworkLogger>(),
//                        std::make_shared<zeromq::ZeroMQCodec>(),
//                        std::make_shared<zeromq::ZeroMQPublisher>(producer)
//                );
//            },
//            "/Users/darvik/server.pem"
//    );

    AsyncClient<TcpSocket> client(
            service,
            [producer](const std::shared_ptr<AsyncChannel<TcpSocket>> &channel) {
                link(
                        channel,
                        std::make_shared<handler::NetworkLogger>(),
                        std::make_shared<zeromq::ZeroMQCodec>(),
                        std::make_shared<zeromq::ZeroMQPublisher>(producer)
                );
            },
            "/Users/darvik/server.pem"
    );

    client.connect("127.0.0.1", port);

    boost::asio::deadline_timer deadline(service);
    deadline.expires_from_now(boost::posix_time::seconds(10));
    deadline.async_wait([producer](boost::system::error_code err) {
        if (!err) {
            producer->publish("joystick", "Hello World");
            producer->publish("test", "Skipped message");
            producer->publish("joystick", "Second message");
        }
    });

    signals.async_wait(
            [&service](boost::system::error_code ec, int signal) {
                if (!ec) {
                    service.stop();
                    app::log::info("service shutdown");
                }
            }
    );

    app::log::info("service started");
    service.run();

    return 0;
}
```

## Application

#### main.cpp

```cpp
#include "Application.h"

class MainApp : public Application {
protected:
    void setup(Registry &registry) override {
        // TODO: init extra libs
        //wiringPiSetup();
        
        // TODO: register own services
        // registry.addService(std::make_shared<I2CServoDriver>());
    }
};

int main(int argc, char *argv[]) {
    MainApp app;
    app.run(argc, argv);
    return 0;
}

```

