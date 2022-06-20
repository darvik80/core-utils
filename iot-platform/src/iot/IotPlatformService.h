//
// Created by Ivan Kishchenko on 26.02.2022.
//

#pragma once

#include "BaseService.h"
#include "network/boost/AsyncTcpClient.h"
#include "IotProperties.h"

#include "event/EventManagerService.h"
#include "network/handler/NetworkLogger.h"
#include "network/handler/IdleStateHandler.h"

#include "network/mqtt/MQTTHandler.h"
#include "network/mqtt/MQTTCodec.h"

#include "../../../../monitor/src/SystemMonitorService.h"
#include "IotPlatform.h"
#include "ThingsBoardIotPlatform.h"

class IotPlatformService : public BaseService {
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
            registry.getService<EventManagerService>()
                    .subscribe<IotTelemetry>(
                            [this](const IotTelemetry &event) -> bool {
                                info("telemetry: {}", event.message);
                                _delegate->telemetry(event.qos, event.message);
                                return true;

                            }
                    );

            _delegate->postConstruct(registry);
        }
    }

    void preDestroy(Registry &registry) override {
        if (_delegate) {
            _delegate->preDestroy(registry);
        }
    }
};


