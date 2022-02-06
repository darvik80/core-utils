//
// Created by Ivan Kishchenko on 10.12.2021.
//

#pragma once

#include <unordered_set>
#include "network/Handler.h"
#include "MQTT.h"

namespace network::mqtt {

    class MQTTHandler
            : public InboundHandler<PublishMessage, PubAckMessage, SubscribeMessage, SubAckMessage>,
              public PrevLink<PublishMessage, PubAckMessage, SubscribeMessage, SubAckMessage> {
    public:
        void handleActive(const Context &ctx) override;

        void handleRead(const Context &ctx, const PublishMessage &msg) override {}

        void handleRead(const Context &ctx, const PubAckMessage &msg) override {}

        void handleRead(const Context &ctx, const SubscribeMessage &msg) override {}

        void handleRead(const Context &ctx, const SubAckMessage &msg) override {}
    };

    class Producer {
    public:
        typedef std::shared_ptr<Producer> Ptr;
    public:
        virtual void publish(std::string_view topic, std::string_view data) = 0;
    };

    class MQTTPublisher : public MQTTHandler, public std::enable_shared_from_this<MQTTPublisher>, public Producer {
        uint16_t _id{1};
    public:
        typedef std::shared_ptr<MQTTPublisher> Ptr;

        explicit MQTTPublisher() = default;

        void publish(std::string_view topic, std::string_view data) override;

        void handleActive(const Context &ctx) override {
            MQTTHandler::handleActive(ctx);
        }

        void handleInactive(const Context &ctx) override {
            MQTTHandler::handleInactive(ctx);
        }

        void handleRead(const Context &ctx, const PublishMessage &msg) override;

        void handleRead(const Context &ctx, const PubAckMessage &msg) override;
    };
}
