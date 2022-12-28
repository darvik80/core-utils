//
// Created by Ivan Kishchenko on 10.07.2021.
//

#pragma once

#include <boost/predef.h>

#ifdef __linux__

#include <linux/joystick.h>
#include "BaseService.h"
#include "JoystickEvent.h"
#include "JoystickProperties.h"

#include <boost/asio/posix/stream_descriptor.hpp>
#include "core-service/EventBusService.h"

typedef std::function<void()> onConnectedCallback;
typedef std::function<void(const boost::system::error_code& err)> onErrorCallback;
typedef std::function<void(const JoystickEvent &event)> onInputCallback;

class JoystickController {
    JoystickEvent _lastEvent;

    js_event _events[64];
    boost::asio::posix::stream_descriptor _stream;
    onConnectedCallback _onConnect;
    onErrorCallback _onError;
    onInputCallback _onInput;
public:
    explicit JoystickController(bus::IOService &service) : _stream(service) {}

    void setOnConnect(const onConnectedCallback &onConnect) {
        _onConnect = onConnect;
    }

    void setOnError(const onErrorCallback &onError) {
        _onError = onError;
    }

    void setOnInput(const onInputCallback &onInput) {
        _onInput = onInput;
    }

    bool setup(const JoystickProperties &props);


    void shutdown();

    ~JoystickController() {
        shutdown();
    }

private:
    static bool extractEventPs3(js_event &event, JoystickEvent &jsEvent);

    static bool extractEventXbox(js_event &event, JoystickEvent &jsEvent);

    static bool extractEventGamepad(js_event &event, JoystickEvent &jsEvent);

    void onRead(JoystickType type, size_t readable);
};

class JoystickService : public BaseServiceShared<JoystickService> {
    std::unique_ptr<JoystickController> _controller;
private:
public:
    const char *name() override {
        return "joystick";
    }

    void postConstruct(Registry &registry) override;

    void preDestroy(Registry &registry) override;

private:
    void setup(Registry &registry);
};

#endif
