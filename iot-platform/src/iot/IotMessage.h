//
// Created by Kishchenko Ivan on 17/06/2022.
//

#pragma once

#include <string>

struct IotMessage {
    std::string topic;
    int qos{0};
    std::string message;
};

struct IotTelemetry {
    int qos{0};
    std::string message;
};