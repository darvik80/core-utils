//
// Created by Kishchenko Ivan on 17/06/2022.
//

#pragma once

#include "IotPlatform.h"
#include "IotCommand.h"

class ThingsBoardIotPlatform : public IotPlatform {
    const std::string TELEMETRY_TOPIC = "v1/devices/me/telemetry";
    const std::string ATTRIBUTES_TOPIC = "v1/devices/me/attributes";
    const std::string CONFIG_TOPIC = "v1/devices/me/config";
    const std::string REQUEST_TOPIC = "v1/devices/me/rpc/request";
    const std::string RESPONSE_TOPIC = "v1/devices/me/rpc/response";
protected:
    void onConnect(network::mqtt::MQTTAgent &agent) override;

public:
    void telemetry(uint8_t qos, std::string_view data) override {
        publish(TELEMETRY_TOPIC, qos, data);
    }


};
