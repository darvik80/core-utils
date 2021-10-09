//
// Created by Ivan Kishchenko on 02.05.2021.
//

#pragma once

#include <utility>
#include <unordered_set>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "SchedulerLogger.h"

class TimerHandler {

    std::shared_ptr<boost::asio::deadline_timer> _timer;
public:
    TimerHandler() = default;

    explicit TimerHandler(boost::asio::io_context &service)
            : _timer(std::make_unique<boost::asio::deadline_timer>(service)) {}

    explicit TimerHandler(std::shared_ptr<boost::asio::deadline_timer> &timer)
            : _timer(std::move(timer)) {
    }

    explicit TimerHandler(const std::weak_ptr<boost::asio::deadline_timer> &timer)
            : _timer(timer) {
    }

    TimerHandler(TimerHandler &other)
            : _timer(std::move(other._timer)) {

    }

    TimerHandler &operator=(const TimerHandler &other) {
        if (&other == this) {
            return *this;
        }
        _timer = other._timer;
        return *this;
    }

    ~TimerHandler() {
        cancel();
    }

public:
    void schedule_once(const boost::posix_time::time_duration &duration, const std::function<void()> &fn) {
        _timer->expires_from_now(duration);
        _timer->async_wait([fn](const boost::system::error_code &ec) {
            if (!ec) {
                fn();
            } else {
                scheduler::log::warning("failed wait timer: {}", ec.message());
            }
        });
    }

    void schedule(const boost::posix_time::time_duration &duration, const std::function<void()> &fn) {
        _timer->expires_from_now(duration);
        _timer->async_wait([fn, duration, this](const boost::system::error_code &ec) {
            if (!ec) {
                fn();
                schedule(duration, fn);
            }
        });
    }

    bool cancel() {
        if (_timer) {
            return _timer->cancel() > 0;
        }
        return false;
    }
};

class Scheduler {
    boost::asio::io_service &_service;

    using Timer = boost::asio::deadline_timer;
    using TimerPtr = std::shared_ptr<Timer>;

public:
    using TimeHandler = std::function<void()>;
    using TimeDuration = boost::posix_time::time_duration;
    using Time = boost::posix_time::ptime;
    using ErrorCode = boost::system::error_code;

    explicit Scheduler(boost::asio::io_service &service)
            : _service(service) {}

    void schedule(const TimeHandler &fn, const TimeDuration &duration);

    void scheduleAtFixedRate(const TimeHandler &fn, const TimeDuration &initDelay, const TimeDuration &period);
    void scheduleWithFixedDelay(const TimeHandler &fn, const TimeDuration &initDelay, const TimeDuration &period);
private:
    void doScheduleAtFixedRate(TimerPtr timer, const TimeHandler &fn, const TimeDuration &period);
    void doScheduleWithFixedDelay(TimerPtr timer, const TimeHandler &fn, const TimeDuration &period);
};

