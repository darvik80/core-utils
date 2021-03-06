//
// Created by Kishchenko Ivan on 17/06/2022.
//

#include "ThingsBoardIotPlatform.h"

using namespace network;

void ThingsBoardIotPlatform::onConnect(mqtt::MQTTAgent &agent) {
    std::string command = REQUEST_TOPIC + "/+";
    agent.callback(ATTRIBUTES_TOPIC, [this](auto &agent, std::string_view topic, std::string_view data) {
        onAttributes(agent, data);
    });
    agent.callback("v1/devices/me/attributes/response/+", [this](auto &agent, std::string_view topic, std::string_view data) {
        onAttributes(agent, data);
    });
    agent.callback(CONFIG_TOPIC, [this](auto &agent, std::string_view topic, std::string_view data) {
        onConfig(agent, data);
    });
    agent.callback(command, [this, command](auto &agent, std::string_view topic, std::string_view data) {
        iot::log::debug("cmd: {}:{}", topic, data);
        std::string response = RESPONSE_TOPIC + topic.substr(REQUEST_TOPIC.size()).data();
        IotCommand cmd;
        from_json(nlohmann::json::parse(data), cmd);
        onCommand(agent, cmd);
        publish(response, 0, "success");
    });
    agent.subscribe(ATTRIBUTES_TOPIC, 1);
    agent.subscribe(CONFIG_TOPIC, 1);
    agent.subscribe(command, 1);

    agent.subscribe("v1/devices/me/attributes/response/+", 1);
    agent.publish("v1/devices/me/attributes/request/1", 1, R"({"sharedKeys":"status"})");
}

