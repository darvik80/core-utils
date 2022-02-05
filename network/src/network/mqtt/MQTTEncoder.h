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

    class MQTTEncoder {
    public:
        typedef std::shared_ptr<MQTTEncoder> Ptr;
        virtual std::error_code write(Buffer &buf, const ConnectMessage &msg) = 0;
        virtual std::error_code write(Buffer &buf, const ConnAckMessage &msg) = 0;
        virtual std::error_code write(Buffer &buf, const PingReqMessage &msg) = 0;
        virtual std::error_code write(Buffer &buf, const PingRespMessage &msg) = 0;
    };
}