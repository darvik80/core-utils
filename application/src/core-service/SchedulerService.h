//
// Created by Ivan Kishchenko on 02.05.2021.
//

#pragma once

#include "BaseService.h"
#include "event/Scheduler.h"

class SchedulerService : public bus::Scheduler, public BaseService {
public:
    explicit SchedulerService(boost::asio::io_service &service)
            : bus::Scheduler(service), BaseService(scheduler_logger::get()) {}

    const char *name() override {
        return "scheduler";
    }

    int order() override {
        return INT32_MAX - 1;
    }
};
