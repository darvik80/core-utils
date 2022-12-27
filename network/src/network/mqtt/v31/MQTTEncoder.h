//
// Created by Ivan Kishchenko on 05.02.2022.
//

#pragma once

#include "network/mqtt/MQTTEncoder.h"

namespace network::mqtt::v31 {

    class MQTTEncoder : public network::mqtt::MQTTEncoder {
    public:
        std::error_code write(Buffer &buf, const ConnectMessage &msg) override;

        std::error_code write(Buffer &buf, const ConnAckMessage &msg) override;

        std::error_code write(Buffer &buf, const PingReqMessage &msg) override;

        std::error_code write(Buffer &buf, const PingRespMessage &msg) override {
            Writer out(buf);

            out << msg.getHeader().all << IOFlag::variable << 0;

            return out.status();
        }

        std::error_code write(Buffer &buf, const PublishMessage &msg) override;

        std::error_code write(Buffer &buf, const PubAckMessage &msg) override;

        std::error_code write(Buffer &buf, const SubscribeMessage &msg) override;

        std::error_code write(Buffer &buf, const SubAckMessage &msg) override;

        std::error_code write(Buffer &buf, const UnSubscribeMessage &msg) override;

        std::error_code write(Buffer &buf, const UnSubAckMessage &msg) override;
    };
}