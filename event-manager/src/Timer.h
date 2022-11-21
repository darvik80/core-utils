//
// Created by Ivan Kishchenko on 02.05.2021.
//

#pragma once

#include <utility>
#include <unordered_set>
#include <boost/system/error_code.hpp>

#include "Asio.h"

namespace em {
    class Timer : public std::enable_shared_from_this<Timer> {
    public:
        typedef std::function<void()> Handler;
        typedef boost::posix_time::time_duration Duration;
    private:
        std::shared_ptr<IOTimer> _timer;
    public:

        Timer() = default;

        explicit Timer(boost::asio::io_context &service)
                : _timer(std::make_unique<IOTimer>(service)) {}

        explicit Timer(std::shared_ptr<IOTimer> &timer)
                : _timer(std::move(timer)) {
        }

        explicit Timer(const std::weak_ptr<IOTimer> &timer)
                : _timer(timer) {
        }

        ~Timer() {
            cancel();
        }

    public:
        void scheduleOnce(const Duration &duration, const Handler &fn);

        void scheduleAtFixedRate(const Duration &duration, const Handler &fn);

        void scheduleWithFixedDelay(const Duration &duration, const Handler &fn);

        bool cancel();
    };
}