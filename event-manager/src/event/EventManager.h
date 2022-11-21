//
// Created by Ivan Kishchenko on 20.12.2020.
//

#pragma once

#include <boost/signals2.hpp>
#include <unordered_map>
#include <typeindex>
#include <any>

#include "EventManagerLogger.h"

#include "Asio.h"
#include "Timer.h"
#include "scheduler/Scheduler.h"

namespace em {
    class EventManager {
    public:
        typedef std::shared_ptr<EventManager> Ptr;
        typedef boost::signals2::signal<void(const std::any &)> Signal;
        typedef std::unordered_map<std::type_index, Signal> SignalMap;
    private:
        IOService &_service;
        SignalMap _signals;
        Scheduler _scheduler;
    public:
        explicit EventManager(IOService &service) : _service(service), _scheduler(service) {}

        template<class E, class H>
        void subscribe(const std::shared_ptr<H> &handler) {
            _signals[typeid(E)].connect(boost::signals2::signal<void(const std::any &)>::slot_type(
                    [handler](const std::any &event) {
                        const auto real = std::any_cast<E>(event);
                        handler->onEvent(real);
                    }
            ).track_foreign(handler));
        }

        template<class E>
        void subscribe(const std::function<bool(const E &)> &callback) {
            _signals[typeid(E)].connect_extended(
                    [callback](const boost::signals2::connection &conn, const std::any &event) {
                        const auto real = std::any_cast<E>(event);
                        if (!callback(real)) {
                            conn.disconnect();
                        }
                    }
            );
        }

        template<class E>
        void send(const E &event) {
            em::log::debug("send event {}", typeid(event).name());
            if (auto iter = _signals.find(typeid(event)); iter != _signals.end()) {
                iter->second(event);
            } else {
                em::log::warning("sent failed event {}, no consumers", typeid(event).name());
            }
        }

        template<class E>
        void post(const E &event) {
            boost::asio::post(_service, [event, this]() {
                send(event);
            });
        }

        template<class E>
        void scheduleOnce(const E &event, Timer::Duration delay) {
            _scheduler.scheduleOnce([this, event]() {
                send(event);
            }, delay);
        }

        template<class E>
        void scheduleWithFixedDelay(const E &event, Timer::Duration initDelay, Timer::Duration period) {
            _scheduler.scheduleWithFixedDelay([this, event]() {
                send(event);
            }, initDelay, period);
        }

        template<class E>
        void scheduleAtFixedRate(const E &event, Timer::Duration initDelay, Timer::Duration period) {
            _scheduler.scheduleAtFixedRate([this, event]() {
                send(event);
            }, initDelay, period);
        }
    };
}
