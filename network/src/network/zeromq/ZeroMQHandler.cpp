//
// Created by Ivan Kishchenko on 15.10.2021.
//

#include "ZeroMQHandler.h"
#include "network/zeromq/ZeroMQLogging.h"

namespace network::zeromq {

    ZeroMQHandler::ZeroMQHandler(std::string_view type)
            : _type(type) {}

    void ZeroMQHandler::handleActive(const Context &ctx) {
        ZeroMQCommand cmd{ZERO_MQ_CMD_READY};
        cmd.props.emplace(ZERO_MQ_PROP_SOCKET_TYPE, _type);
        write(ctx, cmd);
    }

    void ZeroMQSubscriber::handleRead(const Context &ctx, const ZeroMQCommand &cmd) {
        if (cmd.name == ZERO_MQ_CMD_READY) {
            auto socketType = cmd.props.at(ZERO_MQ_PROP_SOCKET_TYPE);
            zeromq::log::info("[sub] handle cmd: {}:{}", cmd.name, socketType);
            if (socketType == ZERO_MQ_SOCKET_TYPE_SUB) {
                fireShutdown();
                return;
            }

            for (auto &callback: _callbacks) {
                zeromq::log::info("[sub] subscribe to: {}", callback.first);
                ZeroMQCommand sub{ZERO_MQ_CMD_SUBSCRIBE};
                sub.props.emplace(ZERO_MQ_PROP_SUBSCRIPTION, callback.first);
                write(ctx, sub);
            }
        }
    }

    void ZeroMQSubscriber::handleRead(const Context &ctx, const ZeroMQMessage &msg) {
        zeromq::log::debug("[sub] handle msg... {}", msg.data.size());
        if (msg.data.size() == 2) {
            zeromq::log::info("[sub] topic: {} ...", msg.data[0]);
            if (auto iter = _callbacks.find(msg.data[0]); iter != _callbacks.end()) {
                iter->second(msg.data[0], msg.data[1]);
            }
        }
    }

    void ZeroMQPublisher::handleRead(const Context &ctx, const ZeroMQCommand &cmd) {
        if (cmd.name == ZERO_MQ_CMD_READY) {
            auto socketType = cmd.props.at(ZERO_MQ_PROP_SOCKET_TYPE);
            zeromq::log::info("[pub] handle cmd: {}:{}", cmd.name, socketType, socketType);

            if (socketType == ZERO_MQ_SOCKET_TYPE_PUB) {
                fireShutdown();
                return;
            }
        } else if (cmd.name == ZERO_MQ_CMD_SUBSCRIBE) {
            auto topic = cmd.props.at(ZERO_MQ_PROP_SUBSCRIPTION);
            _topics.emplace(topic);
            zeromq::log::info("[pub] handle subscribe: {}", topic);
        }
    }

    void ZeroMQPublisher::handleRead(const Context &ctx, const ZeroMQMessage &msg) {
        zeromq::log::debug("[pub] handle msg...");
    }

    void ZeroMQPublisher::publish(std::string_view topic, std::string_view data) {
        if (auto iter = _topics.find(topic.data()); iter != _topics.end()) {
            ZeroMQMessage msg;
            msg << topic << data;
            write(Context{}, msg);
        }
    }
}
