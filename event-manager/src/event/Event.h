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

    class Event {
        EventSource::Ptr _source;
    public:
        Event() = default;

        explicit Event(EventSource::Ptr source)
                : _source(std::move(source)) {}

        [[nodiscard]] EventSource::Ptr source() const {
            return _source;
        }

        virtual ~Event() = default;
    };
}

#endif //ROVER_EVENT_H
