//
// Created by Ivan Kishchenko on 26.02.2022.
//

#pragma once

#include "BaseService.h"
#include "network/boost/AsyncTcpClient.h"

#include "core-service/EventBusService.h"
#include "network/handler/NetworkLogger.h"
#include "network/handler/IdleStateHandler.h"

#include "network/mqtt/MQTTHandler.h"
#include "network/mqtt/MQTTCodec.h"

#include "IotPlatform.h"

class IotPlatformService : public BaseServiceShared<IotPlatformService> {
    std::unique_ptr<IotPlatform> _delegate{};
public:
    const char *name() override {
        return "iot";
    }

    void postConstruct(Registry &registry) override;

    void onEvent(const IotTelemetry &telemetry);

    void onEvent(const IotMessage &msg);


    void preDestroy(Registry &registry) override;
};


