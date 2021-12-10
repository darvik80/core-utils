//
// Created by Ivan Kishchenko on 10.12.2021.
//

#pragma once

#include "network/Handler.h"
#include "MQTT.h"

namespace network::mqtt {

    class MQTTHandler : public InboundHandler<ConnectMessage, ConnAckMessage, PingReqMessage, PingRespMessage>, public PrevLink<ConnectMessage, ConnAckMessage, PingReqMessage, PingRespMessage> {
    public:
        void handleActive(const Context &ctx) override;
    };
}
