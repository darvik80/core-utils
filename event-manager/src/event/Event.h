//
// Created by Ivan Kishchenko on 24.04.2021.
//

#ifndef ROVER_EVENT_H
#define ROVER_EVENT_H

#include <utility>
#include <memory>

namespace em {
    class EventSource {
    public:
        typedef std::shared_ptr<EventSource> Ptr;
    };

    struct Event {
        EventSource::Ptr source{};

        Event() = default;

        explicit Event(const EventSource::Ptr &src) : source(src) {}

        virtual ~Event() = default;
    };
}

#endif //ROVER_EVENT_H
