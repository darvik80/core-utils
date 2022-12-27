//
// Created by Ivan Kishchenko on 27.12.2022.
//


#pragma once

#include "IotPlatform.h"

class CreartsIotPlatform : public IotPlatform {
    const std::string _deviceId;

    std::string _topicConfig;
    std::string _topicCommand;
    std::string _topicTelemetry;
public:
    explicit CreartsIotPlatform(std::string_view deviceId);

    void telemetry(uint8_t qos, std::string_view data) override;

protected:
    void onConnect(network::mqtt::MQTTAgent &agent) override;

    void onPopulateOptions(network::mqtt::MQTTAgent &agent, network::mqtt::MQTTOptions &options) override;
};
