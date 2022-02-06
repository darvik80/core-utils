//
// Created by Ivan Kishchenko on 10.12.2021.
//

#include "MQTTHandler.h"

namespace network::mqtt {

    void MQTTHandler::handleActive(const network::Context &ctx) {
        NetworkHandler::handleActive(ctx);
    }

    void MQTTPublisher::publish(std::string_view topic, std::string_view data) {
        PublishMessage msg(topic, 1, _id++, data);
        write(Context{}, msg);
    }

    void MQTTPublisher::handleRead(const Context &ctx, const PublishMessage &msg) {

    }

    void MQTTPublisher::handleRead(const Context &ctx, const PubAckMessage &msg) {

    }
}
