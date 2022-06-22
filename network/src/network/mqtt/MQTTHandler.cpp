//
// Created by Ivan Kishchenko on 10.12.2021.
//

#include "MQTTHandler.h"
#include "MQTTUtils.h"

namespace network::mqtt {

    void MQTTAgent::handleActive(const network::Context &ctx) {
        NetworkHandler::handleActive(ctx);
        if (_connCallback) {
            _connCallback(*this);
        }
    }

    void MQTTAgent::handleRead(const Context &ctx, const PublishMessage &msg) {
        std::string_view data((const char *) msg.getMessage().data(), msg.getMessage().size());
        for (auto &it: _callbacks) {
            if (MQTTUtils::compareTopics(it.first, msg.getTopic())) {
                it.second(*this, msg.getTopic(), data);
            }
        }
        if (_callback) {
            _callback(*this, msg.getTopic(), data);
        }

        if (msg.getQos() == 1) {
            PubAckMessage reply(msg.getPacketIdentifier());
            write(ctx, reply);
        }
    }

    void MQTTAgent::publish(std::string_view topic, uint8_t qos, std::string_view data) {
        PublishMessage msg(topic, qos, _id++, data);
        write(Context{}, msg);
    }

    void MQTTAgent::subscribe(std::string_view topicFilter, uint8_t qos) {
        SubscribeMessage msg(topicFilter, qos, _id++);
        write(Context{}, msg);
    }

    void MQTTAgent::unSubscribe(std::string_view topicFilter) {
        UnSubscribeMessage msg(topicFilter, _id++);
        write(Context{}, msg);
    }

    void MQTTAgent::connect(const MQTTConnectCallback &fn) {
        _connCallback = fn;
    }

    void MQTTAgent::callback(std::string_view topic, const MQTTMessageCallback &fn) {
        _callbacks.emplace(topic, fn);
    }

    void MQTTAgent::callback(const MQTTMessageCallback &fn) {
        _callback = fn;
    }
}
