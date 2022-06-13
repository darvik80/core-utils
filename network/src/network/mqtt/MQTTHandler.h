//
// Created by Ivan Kishchenko on 10.12.2021.
//

#pragma once

#include <unordered_map>
#include "network/Handler.h"
#include "MQTT.h"


namespace network::mqtt {

    class MQTTHandler
            : public InboundHandler<PublishMessage, PubAckMessage, SubscribeMessage, SubAckMessage, UnSubscribeMessage, UnSubAckMessage>,
              public PrevLink<PublishMessage, PubAckMessage, SubscribeMessage, SubAckMessage, UnSubscribeMessage, UnSubAckMessage> {
    public:
        void handleRead(const Context &ctx, const PublishMessage &msg) override {}

        void handleRead(const Context &ctx, const PubAckMessage &msg) override {}

        void handleRead(const Context &ctx, const SubscribeMessage &msg) override {}

        void handleRead(const Context &ctx, const SubAckMessage &msg) override {}

        void handleRead(const Context &ctx, const UnSubscribeMessage &msg) override {}

        void handleRead(const Context &ctx, const UnSubAckMessage &msg) override {}
    };

    class MQTTAgent;

    typedef std::function<void(MQTTAgent& agent, std::string_view, std::string_view data)> MQTTMessageCallback;
    typedef std::function<void(MQTTAgent& agent)> MQTTConnectCallback;

    class MQTTAgent : public MQTTHandler {
        int _id{1};
    public:
        typedef std::shared_ptr<MQTTAgent> Ptr;
        MQTTConnectCallback _connCallback;

        MQTTMessageCallback _callback;
        std::unordered_map<std::string, MQTTMessageCallback> _callbacks;
    public:
        void handleActive(const Context &ctx) override;

        void handleRead(const Context &ctx, const PublishMessage &msg) override;

        virtual void publish(std::string_view topic, uint8_t qos, std::string_view data);

        virtual void subscribe(std::string_view topicFilter, uint8_t qos);

        virtual void unSubscribe(std::string_view topicFilter);

        virtual void connect(const MQTTConnectCallback& fn);
        virtual void callback(std::string_view topic, const MQTTMessageCallback &fn);
        virtual void callback(const MQTTMessageCallback &fn);
    };
}
