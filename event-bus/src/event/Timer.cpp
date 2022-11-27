//
// Created by Kishchenko Ivan on 21/11/2022.
//
#include "Timer.h"
#include "logging/Logging.h"
#include <system_error>

LOG_COMPONENT_SETUP(timer, timer_logger)

namespace bus {

    void Timer::scheduleOnce(const Timer::Duration &duration, const Timer::Handler &fn) {
        _timer->expires_from_now(duration);
        _timer->async_wait([fn](const std::error_code &ec) {
            if (!ec) {
                fn();
            } else {
                timer::log::warning("scheduleOnce canceled: {}", ec.message());
            }
        });
    }

    void Timer::scheduleAtFixedRate(const Timer::Duration &duration, const Timer::Handler &fn) {
        auto fnSelf = [timer = _timer, duration, fn](auto &selfRef) -> void {
            timer->expires_at(timer->expires_at() + duration);
            timer->async_wait([fn, selfRef](const std::error_code &ec) {
                if (!ec) {
                    fn();
                    selfRef(selfRef);
                } else {
                    timer::log::warning("scheduleAtFixedRate canceled: {}", ec.message());
                }
            });
        };

        scheduleOnce(duration, [fnSelf]() {
            fnSelf(fnSelf);
        });
    }

    void Timer::scheduleWithFixedDelay(const Timer::Duration &duration, const Timer::Handler &fn) {
        auto self = shared_from_this();
        _timer->expires_from_now(duration);
        _timer->async_wait([fn, duration, self = this](const std::error_code &ec) {
            if (!ec) {
                fn();
                self->scheduleWithFixedDelay(duration, fn);
            } else {
                timer::log::warning("scheduleWithFixedDelay canceled: {}", ec.message());
            }
        });
    }

    bool Timer::cancel() {
        if (_timer) {
            return _timer->cancel() > 0;
        }
        return false;
    }
}