//
// Created by Kishchenko Ivan on 17/06/2022.
//

#pragma once

#include <string>
#include "event/Event.h"

struct IotMessage : em::Event {
    IotMessage(std::string_view topic, std::string_view message) : topic(topic), message(message) {}

    std::string topic;
    std::string message;
};

struct IotTelemetry : em::Event {
    IotTelemetry(int qos, std::string_view message) : qos(qos), message(message) {}

    IotTelemetry(int qos, const nlohmann::json &json) : qos(qos) {
        message = json.dump();
    }

    int qos{0};
    std::string message;
};