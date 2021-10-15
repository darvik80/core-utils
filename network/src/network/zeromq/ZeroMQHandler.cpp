//
// Created by Ivan Kishchenko on 15.10.2021.
//

#include "ZeroMQHandler.h"
#include "network/zeromq/ZeroMQLogging.h"

namespace network::zeromq {

    ZeroMQHandler::ZeroMQHandler(std::string_view type)
            : _type(type) {}

    void ZeroMQHandler::handleActive() {
        ZeroMQMsg msg{ZeroMQCommand{ZERO_MQ_CMD_READY}};
        std::get<ZeroMQCommand>(msg).props.emplace(ZERO_MQ_PROP_SOCKET_TYPE, _type);
        write(msg);
    }

    void ZeroMQHandler::handleRead(ZeroMQMsg &event) {
        std::visit([this](auto &&arg) {
            handleRead(arg);
        }, event);
    }

    void ZeroMQSubscriber::handleRead(ZeroMQCommand& cmd) {
        zeromq::log::info("[sub] handle cmd: {}", cmd.name);
        if (cmd.name == ZERO_MQ_CMD_READY) {
            if (cmd.props[ZERO_MQ_PROP_SOCKET_TYPE] == ZERO_MQ_SOCKET_TYPE_SUB) {
                shutdown();
                return;
            }

            for (auto& callback : _callbacks) {
                ZeroMQMsg msg{ZeroMQCommand{ZERO_MQ_CMD_SUBSCRIBE}};
                std::get<ZeroMQCommand>(msg).props.emplace(ZERO_MQ_PROP_SUBSCRIPTION, callback.first);
                write(msg);
            }
        }
    }

    void ZeroMQSubscriber::handleRead(ZeroMQMessage &msg) {
        if (msg.data.size() == 2) {
            if (auto iter = _callbacks.find(msg.data[0]); iter != _callbacks.end()) {
                iter->second(msg.data[0], msg.data[1]);
            }
        }
    }

    void ZeroMQPublisher::handleRead(ZeroMQCommand& cmd) {
        zeromq::log::info("[pub] handle cmd: {}", cmd.name);

        if (cmd.name == ZERO_MQ_CMD_READY) {
            if (cmd.props[ZERO_MQ_PROP_SOCKET_TYPE] == ZERO_MQ_SOCKET_TYPE_PUB) {
                shutdown();
                return;
            }
        } else if (cmd.name == ZERO_MQ_CMD_SUBSCRIBE) {
            _topics.emplace(cmd.props[ZERO_MQ_PROP_SUBSCRIPTION]);
        }
    }

    void ZeroMQPublisher::handleRead(ZeroMQMessage& msg)  {
        zeromq::log::info("[pub] handle msg...");
    }

    void ZeroMQPublisher::publish(std::string_view topic, std::string_view data) {
        ZeroMQMsg msg{ZeroMQMessage{}};
        std::get<ZeroMQMessage>(msg) << topic << data;
        write(msg);
    }
}
