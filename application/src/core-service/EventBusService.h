//
// Created by Ivan Kishchenko on 20.12.2020.
//

#pragma once

#include "event/EventBus.h"
#include "BaseService.h"

class EventBusService : public bus::EventBus, public BaseServiceShared<EventBusService> {
public:
    typedef std::shared_ptr<EventBusService> Ptr;

    explicit EventBusService(bus::IOService &service) : bus::EventBus(service),
                                                        BaseServiceShared<EventBusService>(em_logger::get()) {}

    const char *name() override {
        return "event-bus";
    }

    int order() override {
        return INT_MAX - 1;
    }
};
