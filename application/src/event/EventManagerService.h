//
// Created by Ivan Kishchenko on 20.12.2020.
//

#pragma once

#include <event/EventManager.h>
#include <event/EventManagerLogger.h>
#include "BaseService.h"

class EventManagerService : public em::EventManager, public BaseServiceShared<EventManagerService> {
public:
    EventManagerService() : BaseServiceShared<EventManagerService>(em_logger::get()) {}

    const char *name() override {
        return "event-manager";
    }

    int order() override {
        return INT_MAX - 1;
    }
};
