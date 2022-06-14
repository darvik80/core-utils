//
// Created by Ivan Kishchenko on 10.07.2021.
//

#include "JoystickService.h"

#ifdef __linux__

#include "JoystickLogger.h"

void JoystickService::postConstruct(Registry &registry) {
    BaseService::postConstruct(registry);

    _eventManager = registry.getService<EventManagerService>().shared_from_this();

    auto props = registry.getProperties<JoystickProperties>();

    auto jsType = props.type;

    _stream = std::make_unique<boost::asio::posix::stream_descriptor>(
            registry.getIoService()
    );
    int fd = open(props.name.c_str(), O_RDONLY);
    if (fd > 0) {
        char name[128];
        if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) < 0) {
            strncpy(name, "unknown", sizeof(name));
        }
        joy::log::info("joystick name: {}", name);
        if (props.type == JoystickType::detect) {
            if (0 == strcmp("PS3 Controller", name)) {
                jsType = JoystickType::ps3;
            } else if (0 == strcmp("Gamepad", name)) {
                jsType = JoystickType::gamepad;
            } else {
                jsType = JoystickType::xbox;
            }
        }

        _stream->assign(fd);
        _stream->async_read_some(
                boost::asio::buffer(&_events, sizeof(_events)),
                [this, jsType](const boost::system::error_code error, std::size_t readable) {
                    if (!error) {
                        onRead(jsType, readable);
                    } else {
                        // TODO: reopen joystick
                    }
                }
        );
    } else {
        joy::log::warning("failed to open joystick");
    }
}

bool JoystickService::extractEventPs3(js_event &event, JoystickEvent &jsEvent) {
    bool changed = false;
    joy::log::info("ev: {}, {}, {}", event.number, event.type, event.value);
    if (event.number == 0 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_left].x = event.value;
        changed = true;
    } else if (event.number == 3 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_right].x = event.value;
        changed = true;
    } else if (event.number == 1 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_left].y = event.value;
        changed = true;
    } else if (event.number == 4 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_right].y = event.value;
        changed = true;
    } else if (event.number == 13 && JS_EVENT_BUTTON == event.type) {
        jsEvent.axis[AxisId::axis_mid].y = event.value ? -32767 : 0;
        changed = true;
    } else if (event.number == 14 && JS_EVENT_BUTTON == event.type) {
        jsEvent.axis[AxisId::axis_mid].y = event.value ? 32767 : 0;
        changed = true;
    } else if (event.number == 15 && JS_EVENT_BUTTON == event.type) {
        jsEvent.axis[AxisId::axis_mid].x = event.value ? -32767 : 0;
        changed = true;
    } else if (event.number == 16 && JS_EVENT_BUTTON == event.type) {
        jsEvent.axis[AxisId::axis_mid].y = event.value ? 32767 : 0;
        changed = true;
    } else if (event.number == 2 && JS_EVENT_AXIS == event.type) {
        jsEvent.lt = event.value;
        changed = true;
    } else if (event.number == 5 && JS_EVENT_AXIS == event.type) {
        jsEvent.rt = event.value;
        changed = true;
    } else if (event.number == 4 && JS_EVENT_BUTTON == event.type) {
        jsEvent.lb = event.value;
        changed = true;
    } else if (event.number == 5 && JS_EVENT_BUTTON == event.type) {
        jsEvent.rb = event.value;
        changed = true;
    } else if (event.number == 0 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnA = event.value;
        changed = true;
    } else if (event.number == 3 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnX = event.value;
        changed = true;
    } else if (event.number == 2 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnY = event.value;
        changed = true;
    } else if (event.number == 1 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnB = event.value;
        changed = true;
    } else if (event.number == 8 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnBack = event.value;
        changed = true;
    } else if (event.number == 10 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnXbox = event.value;
        changed = true;
    } else if (event.number == 9 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnStart = event.value;
        changed = true;
    }

    return changed;
}

bool JoystickService::extractEventXbox(js_event &event, JoystickEvent &jsEvent) {
    bool changed = false;
    joy::log::info("ev: {}, {}, {}", event.number, event.type, event.value);
    if (event.number == 0 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_left].x = event.value;
        changed = true;
    } else if (event.number == 3 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_right].x = event.value;
        changed = true;
    } else if (event.number == 1 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_left].y = event.value;
        changed = true;
    } else if (event.number == 4 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_right].y = event.value;
        changed = true;
    } else if (event.number == 6 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_mid].x = event.value;
        changed = true;
    } else if (event.number == 7 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_mid].y = event.value;
        changed = true;
    } else if (event.number == 2 && JS_EVENT_AXIS == event.type) {
        jsEvent.lt = event.value;
        changed = true;
    } else if (event.number == 5 && JS_EVENT_AXIS == event.type) {
        jsEvent.rt = event.value;
        changed = true;
    } else if (event.number == 4 && JS_EVENT_BUTTON == event.type) {
        jsEvent.lb = event.value;
        changed = true;
    } else if (event.number == 5 && JS_EVENT_BUTTON == event.type) {
        jsEvent.rb = event.value;
        changed = true;
    } else if (event.number == 0 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnA = event.value;
        changed = true;
    } else if (event.number == 2 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnX = event.value;
        changed = true;
    } else if (event.number == 3 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnY = event.value;
        changed = true;
    } else if (event.number == 1 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnB = event.value;
        changed = true;
    } else if (event.number == 6 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnBack = event.value;
        changed = true;
    } else if (event.number == 8 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnXbox = event.value;
        changed = true;
    } else if (event.number == 7 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnStart = event.value;
        changed = true;
    }

    return changed;
}

bool JoystickService::extractEventGamepad(js_event &event, JoystickEvent &jsEvent) {
    bool changed = false;
    joy::log::info("ev: {}, {}, {}", event.number, event.type, event.value);
    if (event.number == 1 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_left].y = event.value;
        changed = true;
    } else if (event.number == 0 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_right].x = event.value;
        changed = true;
    } else if (event.number == 3 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_right].y = event.value;
        changed = true;
    } else if (event.number == 2 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_right].x = event.value;
        changed = true;
    } else if (event.number == 3 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnX = event.value;
        changed = true;
    } else if (event.number == 4 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnY = event.value;
        changed = true;
    } else if (event.number == 0 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnA = event.value;
        changed = true;
    } else if (event.number == 1 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnB = event.value;
        changed = true;
    } else if (event.number == 6 && JS_EVENT_BUTTON == event.type) {
        jsEvent.lb = event.value;
        changed = true;
    } else if (event.number == 7 && JS_EVENT_BUTTON == event.type) {
        jsEvent.rb = event.value;
        changed = true;
    } else if (event.number == 5 && JS_EVENT_AXIS == event.type) {
        jsEvent.lt = event.value;
        changed = true;
    } else if (event.number == 4 && JS_EVENT_AXIS == event.type) {
        jsEvent.rt = event.value;
        changed = true;
    } else if (event.number == 6 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_mid].x = event.value;
        changed = true;
    } else if (event.number == 7 && JS_EVENT_AXIS == event.type) {
        jsEvent.axis[AxisId::axis_mid].y = event.value;
        changed = true;
    } else if (event.number == 10 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnBack = event.value;
        changed = true;
    } else if (event.number == 11 && JS_EVENT_BUTTON == event.type) {
        jsEvent.btnStart = event.value;
        changed = true;
    }

    return changed;
}

void JoystickService::onRead(JoystickType type, size_t readable) {
    JoystickEvent event = _lastEvent;
    std::size_t size = readable / sizeof(js_event);
    bool changed = false;
    for (int idx = 0; idx < size; idx++) {
        event.type = type;
        switch (type) {
            case JoystickType::ps3:
                changed |= extractEventPs3(_events[idx], event);
                break;
            case JoystickType::gamepad:
                changed |= extractEventGamepad(_events[idx], event);
                break;
            case JoystickType::xbox:
            default:
                changed |= extractEventXbox(_events[idx], event);
        }
    }

    if (changed) {
        _eventManager->raiseEvent(event);
    }

    _lastEvent = event;
    _stream->async_read_some(
            boost::asio::buffer(&_events, sizeof(_events)),
            [this, type](const boost::system::error_code error, std::size_t readable) {
                if (!error) {
                    onRead(type, readable);
                } else {
                    // TODO: reopen joystick
                }
            }
    );
}

void JoystickService::preDestroy(Registry &registry) {
    BaseService::preDestroy(registry);
}

#endif