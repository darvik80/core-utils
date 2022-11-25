//
// Created by Ivan Kishchenko on 10.07.2021.
//

#pragma once

#include <boost/predef.h>

#ifdef __linux__

#include <linux/joystick.h>
#include "BaseService.h"
#include "JoystickEvent.h"
#include <boost/asio/posix/stream_descriptor.hpp>
#include "core-service/EventBusService.h"

class JoystickService : public BaseServiceShared<JoystickService> {
    js_event _events[64];
    std::unique_ptr<boost::asio::posix::stream_descriptor> _stream;

    JoystickEvent _lastEvent;
    EventBusService::Ptr _eventManager;

private:
    bool extractEventXbox(js_event &event, JoystickEvent &jsEvent);

    bool extractEventPs3(js_event &event, JoystickEvent &jsEvent);

    bool extractEventGamepad(js_event &event, JoystickEvent &jsEvent);

public:
    const char *name() override {
        return "joystick";
    }

    void postConstruct(Registry &registry) override;

    void preDestroy(Registry &registry) override;

private:
    void onRead(JoystickType type, size_t readable);
};

#endif