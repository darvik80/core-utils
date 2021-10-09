//
// Created by Ivan Kishchenko on 02.05.2021.
//

#include "Scheduler.h"


void Scheduler::schedule(const TimeHandler &fn, const TimeDuration &duration) {
    TimerPtr timer = std::make_shared<Timer>(_service, duration);
    timer->async_wait([fn, timer](const ErrorCode &ec) {
        if (!ec) {
            fn();
        }
    });
}

void Scheduler::doScheduleWithFixedDelay(TimerPtr timer, const TimeHandler &fn, const TimeDuration &period) {
    fn();

    timer->expires_from_now(period);
    timer->async_wait([fn, timer, period, this](const ErrorCode &ec) {
        if (!ec) {
            doScheduleWithFixedDelay(timer, fn, period);
        }
    });
}

void Scheduler::scheduleWithFixedDelay(const TimeHandler &fn, const TimeDuration &initDelay, const TimeDuration &period) {
    TimerPtr timer = std::make_shared<Timer>(_service, initDelay);
    timer->async_wait([fn, timer, period, this](const ErrorCode &ec) {
        if (!ec) {
            doScheduleAtFixedRate(timer, fn, period);
        }
    });
}

void Scheduler::doScheduleAtFixedRate(TimerPtr timer, const TimeHandler &fn, const TimeDuration &period) {
    fn();

    timer->expires_at(timer->expires_at()+period);
    timer->async_wait([fn, timer, period, this](const ErrorCode &ec) {
        if (!ec) {
            doScheduleAtFixedRate(timer, fn, period);
        }
    });
}

void Scheduler::scheduleAtFixedRate(const TimeHandler &fn, const TimeDuration &initDelay, const TimeDuration &period) {
    TimerPtr timer = std::make_shared<Timer>(_service, initDelay);
    timer->async_wait([fn, timer, period, this](const ErrorCode &ec) {
        if (!ec) {
            doScheduleAtFixedRate(timer, fn, period);
        }
    });
}
