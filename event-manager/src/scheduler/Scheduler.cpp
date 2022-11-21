//
// Created by Ivan Kishchenko on 02.05.2021.
//

#include "Scheduler.h"

namespace em {

    void Scheduler::scheduleOnce(const Timer::Handler &fn, Timer::Duration delay) {
        auto timer = std::make_shared<Timer>(_service);
        timer->scheduleOnce(delay, [this, timer, fn]() {
            fn();
        });
    }

    void Scheduler::scheduleWithFixedDelay(const Timer::Handler &fn, Timer::Duration initDelay, Timer::Duration period) {
        auto timer = std::make_shared<Timer>(_service);
        if (initDelay.is_positive()) {
            timer->scheduleOnce(initDelay, [this, period, timer, fn]() {
                fn();
                timer->scheduleWithFixedDelay(period, [this, timer, fn]() {
                    fn();
                });
            });

        } else {
            timer->scheduleWithFixedDelay(period, [this, timer, fn]() {
                fn();
            });
        }
    }

    void Scheduler::scheduleAtFixedRate(const Timer::Handler &fn, Timer::Duration initDelay, Timer::Duration period) {
        auto timer = std::make_shared<Timer>(_service);
        if (initDelay.is_positive()) {
            timer->scheduleOnce(initDelay, [this, period, timer, fn]() {
                fn();
                timer->scheduleAtFixedRate(period, [this, timer, fn]() {
                    fn();
                });
            });
        } else {
            timer->scheduleAtFixedRate(period, [this, timer, fn]() {
                fn();
            });
        }
    }

}
