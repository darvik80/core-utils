//
// Created by Ivan Kishchenko on 10.07.2021.
//

#pragma once

#include "BaseService.h"
#include <boost/asio/posix/stream_descriptor.hpp>
#include <linux/joystick.h>
#include "event/EventManagerService.h"
#include "JoystickEvent.h"

class JoystickService : public BaseServiceShared<JoystickService> {
    js_event _events[64];
    std::unique_ptr<boost::asio::posix::stream_descriptor> _stream;

    JoystickEvent _lastEvent;
    em::EventManager::Ptr _eventManager;

private:
    bool extractEventXbox(js_event& event, JoystickEvent& jsEvent);
    bool extractEventPs3(js_event& event, JoystickEvent& jsEvent);
public:
    const char *name() override {
        return "joystick";
    }

    void postConstruct(Registry &registry) override;

    void preDestroy(Registry &registry) override;

private:
    void onRead(JoystickType type, size_t readable);
};
