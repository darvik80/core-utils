//
// Created by Ivan Kishchenko on 26.02.2022.
//

#pragma once

#include "BaseService.h"
#include "network/boost/AsyncTcpClient.h"

#include "event/EventManagerService.h"
#include "network/handler/NetworkLogger.h"
#include "network/handler/IdleStateHandler.h"

#include "network/mqtt/MQTTHandler.h"
#include "network/mqtt/MQTTCodec.h"

#include "IotPlatform.h"
#include "IotProperties.h"
#include "ThingsBoardIotPlatform.h"

class IotPlatformService : public BaseServiceShared<IotPlatformService> {
    std::unique_ptr<IotPlatform> _delegate;
public:
    const char *name() override {
        return "iot";
    }

    void postConstruct(Registry &registry) override {
        BaseService::postConstruct(registry);

        auto props = registry.getProperties<IotProperties>();
        switch (props.type) {
            case IotType::ThingsBoard:
                _delegate = std::make_unique<ThingsBoardIotPlatform>();
                break;
            case IotType::Yandex:
            default:
                _delegate.reset();
                break;

        }
        if (_delegate) {
            registry.getService<EventManagerService>().subscribe<IotTelemetry>(shared_from_this());
            registry.getService<EventManagerService>().subscribe<IotMessage>(shared_from_this());
            _delegate->postConstruct(registry);
        }
    }

    void onEvent(const IotTelemetry &telemetry) {
        debug("telemetry: {}", telemetry.message);
        _delegate->telemetry(telemetry.qos, telemetry.message);
    }

    void onEvent(const IotMessage &msg) {
        debug("publish: {}", msg.message);
        _delegate->publish(msg.topic, msg.qos, msg.message);
    }


    void preDestroy(Registry &registry) override {
        if (_delegate) {
            _delegate->preDestroy(registry);
        }
    }
};


