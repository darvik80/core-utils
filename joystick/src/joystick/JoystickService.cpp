//
// Created by Ivan Kishchenko on 10.07.2021.
//

#include "JoystickService.h"

#ifdef __linux__

#include "JoystickLogger.h"
#include "core-service/SchedulerService.h"

bool JoystickController::setup(const JoystickProperties &props) {
    int fd = open(props.name.c_str(), O_RDONLY);
    if (fd > 0) {
        char name[128];
        if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) < 0) {
            strncpy(name, "unknown", sizeof(name));
        }
        joy::log::info("detect joystick name: {}", name);

        auto jsType = props.type;
        if (props.type == JoystickType::detect) {
            if (0 == strcmp("PS3 Controller", name)) {
                jsType = JoystickType::ps3;
            } else if (0 == strcmp("Gamepad", name)) {
                jsType = JoystickType::gamepad;
            } else {
                jsType = JoystickType::xbox;
            }
        }

        _stream.assign(fd);
        _stream.async_read_some(
                boost::asio::buffer(&_events, sizeof(_events)),
                [this, jsType](const boost::system::error_code error, std::size_t readable) {
                    if (!error) {
                        onRead(jsType, readable);
                    } else {
                        if (_onError) {
                            _onError(error);
                        }
                    }
                }
        );

        if (_onConnect) {
            _onConnect();
        }
    } else {
        if (_onError) {
            _onError(boost::system::errc::make_error_code(boost::system::errc::io_error));
        }
    }

    return _stream.is_open();
}

bool JoystickController::extractEventPs3(js_event &event, JoystickEvent &jsEvent) {
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

bool JoystickController::extractEventXbox(js_event &event, JoystickEvent &jsEvent) {
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

bool JoystickController::extractEventGamepad(js_event &event, JoystickEvent &jsEvent) {
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

void JoystickController::onRead(JoystickType type, size_t readable) {
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
        //_eventManager->send(event);
        if (_onInput) {
            _onInput(event);
        }
    }

    _lastEvent = event;
    _stream.async_read_some(
            boost::asio::buffer(&_events, sizeof(_events)),
            [this, type](const boost::system::error_code error, std::size_t readable) {
                if (!error) {
                    onRead(type, readable);
                } else {
                    if (_onError) {
                        _onError(error);
                    }
                }
            }
    );
}

void JoystickController::shutdown() {
    if (_stream.is_open()) {
        _stream.close();
        joy::log::info("close & shutdown");
    }
}

void JoystickService::preDestroy(Registry &registry) {
    BaseService::preDestroy(registry);
}

void JoystickService::setup(Registry &registry) {
    auto eventManager = registry.getService<EventBusService>().shared_from_this();
    auto& scheduler = registry.getService<SchedulerService>();

    _controller = std::make_unique<JoystickController>(registry.getIoService());
    _controller->setOnInput([eventManager](const JoystickEvent &event) {
        eventManager->send(event);
    });

    _controller->setOnError([&scheduler, &registry, this](const boost::system::error_code &err) {
        joy::log::warning("handle err: {}", err.message());
        scheduler.scheduleOnce([&registry, this]() {
            setup(registry);
        }, boost::posix_time::seconds{10});
    });

    _controller->setup(registry.getProperties<JoystickProperties>());
}

void JoystickService::postConstruct(Registry &registry) {
    BaseService::postConstruct(registry);

    setup(registry);
}

#endif
