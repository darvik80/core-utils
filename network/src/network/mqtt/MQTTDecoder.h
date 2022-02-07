//
// Created by Ivan Kishchenko on 05.02.2022.
//

#pragma once

#include <memory>
#include <system_error>
#include <functional>

#include "MQTT.h"
#include "network/Buffer.h"


namespace network::mqtt {

    class MQTTDecoder {
    protected:
        std::function<void(const ConnectMessage &msg)> _connHandler;
        std::function<void(const ConnAckMessage &msg)> _connAckHandler;
        std::function<void(const PingReqMessage &msg)> _pingHandler;
        std::function<void(const PingRespMessage &msg)> _pongHandler;
        std::function<void(const PublishMessage &msg)> _pubHandler;
        std::function<void(const PubAckMessage &msg)> _pubAckHandler;
        std::function<void(const SubscribeMessage &msg)> _subHandler;
        std::function<void(const SubAckMessage &msg)> _subAckHandler;
        std::function<void(const UnSubscribeMessage &msg)> _unSubHandler;
        std::function<void(const UnSubAckMessage &msg)> _unSubAckHandler;
    public:
        typedef std::shared_ptr<MQTTDecoder> Ptr;

        virtual std::error_code read(Buffer &buf) = 0;

        void onConnect(const std::function<void(const ConnectMessage &msg)> &handler) {
            _connHandler = handler;
        }

        void onConnAck(const std::function<void(const ConnAckMessage &msg)> &handler) {
            _connAckHandler = handler;
        }

        void onPing(const std::function<void(const PingReqMessage &msg)> &handler) {
            _pingHandler = handler;
        }

        void onPong(const std::function<void(const PingRespMessage &msg)> &handler) {
            _pongHandler = handler;
        }

        void onPublish(const std::function<void(const PublishMessage &msg)> &handler) {
            _pubHandler = handler;
        }

        void onPubAck(const std::function<void(const PubAckMessage &msg)> &handler) {
            _pubAckHandler = handler;
        }

        void onSubscribe(const std::function<void(const SubscribeMessage &msg)> &handler) {
            _subHandler = handler;
        }

        void onSubAck(const std::function<void(const SubAckMessage &msg)> &handler) {
            _subAckHandler = handler;
        }

        void onUnSubscribe(const std::function<void(const UnSubscribeMessage &msg)> &handler) {
            _unSubHandler = handler;
        }

        void onUnSubAck(const std::function<void(const UnSubAckMessage &msg)> &handler) {
            _unSubAckHandler = handler;
        }
    };
}

