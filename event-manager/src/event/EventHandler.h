//
// Created by Ivan Kishchenko on 24.04.2021.
//

#pragma once

#include "Event.h"

namespace em {
    class EventHandler {
    public:
        typedef std::shared_ptr<EventHandler> Ptr;

        virtual ~EventHandler() = default;
    };

    template<class T>
    class TEventHandler : virtual public EventHandler {
    public:
        virtual void onEvent(const T &event) = 0;

        ~TEventHandler() override = default;
    };
}

