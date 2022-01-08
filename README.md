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

## Application

#### main.cpp

# include "Application.h"

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

