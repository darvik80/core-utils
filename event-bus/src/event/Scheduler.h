//
// Created by Ivan Kishchenko on 02.05.2021.
//

#pragma once

#include <utility>
#include <unordered_set>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "Timer.h"
#include "logging/Logging.h"

LOG_COMPONENT_SETUP(scheduler, scheduler_logger)

namespace bus {
    class Scheduler {
        boost::asio::io_service &_service;
    public:
        using ErrorCode = boost::system::error_code;

        explicit Scheduler(boost::asio::io_service &service)
                : _service(service) {}

        void scheduleOnce(const Timer::Handler &fn, Timer::Duration delay);

        void scheduleAtFixedRate(const Timer::Handler &fn, Timer::Duration initDelay, Timer::Duration period);

        void scheduleWithFixedDelay(const Timer::Handler &fn, Timer::Duration initDelay, Timer::Duration period);
    };

}