//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include "network/Handler.h"
#include "network/zeromq/ZeroMQ.h"
#include <utility>
#include <variant>
#include <unordered_map>
#include <unordered_set>

namespace network::zeromq {

    class ZeroMQHandler
            : public InboundHandler<ZeroMQCommand, ZeroMQMessage>, public PrevLink<ZeroMQCommand, ZeroMQMessage> {
        std::string _type;
    public:
        explicit ZeroMQHandler(std::string_view type);

        void handleActive(const Context &ctx) override;
    };

    typedef std::function<void(std::string_view topic, std::string_view data)> fnOnSubMessage;

    class ZeroMQSubscriber : public ZeroMQHandler {
        std::unordered_map<std::string, fnOnSubMessage> _callbacks;
    public:
        ZeroMQSubscriber()
                : ZeroMQHandler(ZERO_MQ_SOCKET_TYPE_SUB) {}

        void subscribe(std::string_view topic, const fnOnSubMessage &callback) {
            _callbacks.emplace(topic, callback);
        }

        void handleRead(const Context &ctx, const ZeroMQCommand &cmd) override;

        void handleRead(const Context &ctx, const ZeroMQMessage &msg) override;
    };

    class Producer {
    public:
        typedef std::shared_ptr<Producer> Ptr;
    public:
        virtual void publish(std::string_view topic, std::string_view data) = 0;
    };

    class CompositeProducer : public Producer {
        std::unordered_set<Producer::Ptr> _pubs;
    public:
        typedef std::shared_ptr<CompositeProducer> Ptr;

        virtual void add(Producer::Ptr pub) {
            _pubs.emplace(pub);
        }

        virtual void remove(Producer::Ptr pub) {
            _pubs.erase(pub);
        }

        void publish(std::string_view topic, std::string_view data) override {
            for (const auto &pub: _pubs) {
                pub->publish(topic, data);
            }
        }
    };

    class ZeroMQPublisher
            : public ZeroMQHandler, public std::enable_shared_from_this<ZeroMQPublisher>, public Producer {
        CompositeProducer::Ptr _pub;
        std::unordered_set<std::string> _topics;
    public:
        typedef std::shared_ptr<ZeroMQPublisher> Ptr;

        explicit ZeroMQPublisher(CompositeProducer::Ptr pub)
                : ZeroMQHandler(ZERO_MQ_SOCKET_TYPE_PUB), _pub(std::move(pub)) {}

        void publish(std::string_view topic, std::string_view data) override;

        void handleActive(const Context &ctx) override {
            _pub->add(shared_from_this());
            ZeroMQHandler::handleActive(ctx);
        }

        void handleInactive(const Context &ctx) override {
            ZeroMQHandler::handleInactive(ctx);
            _pub->remove(shared_from_this());
        }

        void handleRead(const Context &ctx, const ZeroMQCommand &cmd) override;

        void handleRead(const Context &ctx, const ZeroMQMessage &msg) override;
    };
}


