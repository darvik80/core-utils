//
// Created by Ivan Kishchenko on 01.08.2021.
//

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