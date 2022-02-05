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
    };
}

