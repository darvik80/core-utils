//
// Created by Ivan Kishchenko on 01.08.2021.
//

#include "event/EventManagerLogger.h"
#include "Timer.h"
#include "event/EventManager.h"

struct MyEvent  {
    std::string msg;
};

class MyEventHandler {
public:
    void onEvent(const MyEvent &event) {
        logger::info("handle: {}", event.msg);
    }
};

int main(int argc, char *argv[]) {
    logger::LoggingProperties logProps;
    logProps.level = "debug";
    logger::setup(logProps);

    em::IOService service;
    em::EventManager mng(service);

    mng.subscribe<std::string>([](const std::string &event) -> bool {
        em::log::info("handle event: {}", event);
        return true;
    });

    mng.scheduleAtFixedRate(std::string("scheduleAtFixedRate"), boost::posix_time::seconds{0}, boost::posix_time::seconds{10});

    mng.scheduleWithFixedDelay(std::string("scheduleWithFixedDelay"), boost::posix_time::seconds{0}, boost::posix_time::seconds{5});

    mng.scheduleOnce(std::string("scheduleOnce"), boost::posix_time::seconds{10});

    mng.send(std::string("Hello"));
    mng.post(std::string("Hello from asio"));

    auto myHandler = std::make_shared<MyEventHandler>();

    mng.subscribe<MyEvent>(myHandler);

    mng.post(MyEvent{.msg = "hello"});

    service.run();
    return 0;
}