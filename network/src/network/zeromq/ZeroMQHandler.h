//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include "network/Handler.h"
#include "network/zeromq/ZeroMQ.h"
#include <variant>
#include <unordered_map>
#include <unordered_set>

namespace network::zeromq {

    class ZeroMQHandler : public NetworkHandler, public InboundHandler<ZeroMQMsg>, public NetworkWriter<ZeroMQMsg> {
        std::string _type;
    public:
        explicit ZeroMQHandler(std::string_view type);

        void handleActive() override;
        void handleRead(std::variant<ZeroMQCommand, ZeroMQMessage> &event) override;

        virtual void handleRead(ZeroMQCommand& cmd) = 0;
        virtual void handleRead(ZeroMQMessage& msg) = 0;
    };

    typedef std::function<void(std::string_view topic, std::string_view data)> fnOnSubMessage;

    class ZeroMQSubscriber : public ZeroMQHandler {
        std::unordered_map<std::string, fnOnSubMessage> _callbacks;
    public:
        ZeroMQSubscriber() : ZeroMQHandler(ZERO_MQ_SOCKET_TYPE_SUB) {}

        void subscribe(std::string_view topic, const fnOnSubMessage& callback) {
            _callbacks.emplace(topic, callback);
        }

        void handleRead(ZeroMQCommand& cmd) override;
        void handleRead(ZeroMQMessage& msg) override;
    };

    class ZeroMQPublisher : public ZeroMQHandler {
        std::unordered_set<std::string> _topics;
    public:
        ZeroMQPublisher() : ZeroMQHandler(ZERO_MQ_SOCKET_TYPE_PUB) {}

        void publish(std::string_view topic, std::string_view data);

        void handleRead(ZeroMQCommand& cmd) override;
        void handleRead(ZeroMQMessage& msg) override;
    };
}


