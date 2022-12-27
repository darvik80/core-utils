//
// Created by Ivan Kishchenko on 27.12.2022.
//

#include "CreartsIotPlatform.h"
#include <fmt/core.h>
#include "IotPlatformLogger.h"

CreartsIotPlatform::CreartsIotPlatform(std::string_view deviceId) : _deviceId(deviceId) {
    std::string prefix = "v1/devices/" + _deviceId;
    _topicConfig = prefix + "/config";
    _topicCommand = prefix + "/command";
    _topicTelemetry = prefix + "/telemetry";
}

void CreartsIotPlatform::onConnect(network::mqtt::MQTTAgent &agent) {
    agent.callback(_topicConfig, [this](auto &agent, std::string_view topic, std::string_view data) {
        onConfig(agent, data);
    });
    agent.callback(_topicCommand, [this](auto &agent, std::string_view topic, std::string_view data) {
        iot::log::debug("cmd: {}:{}", topic, data);
        IotCommand cmd;
        from_json(nlohmann::json::parse(data), cmd);
        onCommand(agent, cmd);
    });

    agent.subscribe(_topicConfig, 1);
    agent.subscribe(_topicCommand, 1);

    publish(_topicTelemetry, 0,  R"({ "state":"online" })");
}

void CreartsIotPlatform::telemetry(uint8_t qos, std::string_view data) {
    publish(_topicTelemetry, qos, data);
}

void CreartsIotPlatform::onPopulateOptions(network::mqtt::MQTTAgent &agent, network::mqtt::MQTTOptions &options) {
    IotPlatform::onPopulateOptions(agent, options);
    options.willTopic = _topicCommand;
    options.willMessage = R"({ "state":"offline" })";
}
