//
// Created by Ivan Kishchenko on 20.12.2020.
//

#pragma once

#include <boost/signals2.hpp>
#include <unordered_map>
#include <typeindex>

#include "EventHandler.h"
#include "logging/Logging.h"

namespace em {

class EventManager {
public:
    typedef std::shared_ptr<EventManager> Ptr;
    typedef boost::signals2::signal<void(const Event &)> Signal;
private:
    typedef std::unordered_map<std::type_index, Signal> SignalMap;
    SignalMap _signals;
public:
    template<class E>
    void subscribe(const EventHandler::Ptr &handler) {
        auto *channel = dynamic_cast< TEventHandler<E> *>(handler.get());
        if (!channel) {
            throw std::invalid_argument(std::string("[EM] handler not support event ") + typeid(E).name());
        }

        _signals[typeid(E)].connect(boost::signals2::signal<void(const Event &)>::slot_type(
                [channel](const Event &event) {
                    const auto *real = static_cast<const E *>(&event);
                    if (real) {
                        channel->onEvent(*real);
                    }
                }
        ).track_foreign(handler));
    }

    template<class E>
    boost::signals2::connection subscribe(const std::function<bool(const E &)> &callback) {
        return _signals[typeid(E)].connect_extended(
                [callback](const boost::signals2::connection &conn, const Event &event) {
                    const auto *real = static_cast<const E *>(&event);
                    if (!callback(*real)) {
                        conn.disconnect();
                    }
                }
        );
    }

    void raiseEvent(const Event &event);
};

}
