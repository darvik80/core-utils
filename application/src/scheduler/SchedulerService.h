//
// Created by Ivan Kishchenko on 02.05.2021.
//

#pragma once

#include "BaseService.h"
#include <scheduler/Scheduler.h>

class SchedulerService : public Scheduler, public BaseService {
public:
    explicit SchedulerService(boost::asio::io_service &service)
            : Scheduler(service) {}

    const char *name() override {
        return "scheduler";
    }

    int order() override {
        return INT32_MAX - 1;
    }
};
