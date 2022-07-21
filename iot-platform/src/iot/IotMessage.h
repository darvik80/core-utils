//
// Created by Kishchenko Ivan on 17/06/2022.
//

#pragma once

#include <string>
#include "event/Event.h"

struct IotMessage : em::Event {
    IotMessage(std::string_view topic, int qos, std::string_view message) : topic(topic), qos(qos), message(message) {}

    std::string topic;
    int qos{0};
    std::string message;
};

struct IotTelemetry : em::Event {
    IotTelemetry() = default;
    IotTelemetry(int qos, std::string_view message) : qos(qos), message(message) {}

    int qos{0};
    std::string message;
};