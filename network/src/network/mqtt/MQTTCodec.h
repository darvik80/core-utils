//
// Created by Ivan Kishchenko on 10.12.2021.
//

#pragma once

#include "network/Handler.h"
#include "network/Buffer.h"
#include <memory>
#include <variant>

#include "MQTT.h"
#include "MQTTDecoder.h"
#include "MQTTEncoder.h"

namespace network::mqtt {

    struct MQTTOptions {
        std::string clientId;
        std::string accessToken;
        std::string username;
        std::string password;
        std::string willTopic;
        std::string willMessage;
    };

    class MQTTCodec
            : public InboundOutboundMessageHandler<Buffer, PublishMessage, PubAckMessage, SubscribeMessage, SubAckMessage, UnSubscribeMessage, UnSubAckMessage> {
        ArrayBuffer<16384> _incBuf;
        MQTTDecoder::Ptr _decoder;
        MQTTEncoder::Ptr _encoder;

        MQTTOptions _options;
    public:
        explicit MQTTCodec(const MQTTOptions &options);

        void handleActive(const Context &ctx) override;

        void handleRead(const Context &ctx, const Buffer &msg) override;

        void handleWrite(const Context &ctx, const PublishMessage &msg) override;

        void handleWrite(const Context &ctx, const PubAckMessage &msg) override;

        void handleWrite(const Context &ctx, const SubscribeMessage &msg) override;

        void handleWrite(const Context &ctx, const SubAckMessage &msg) override;

        void handleWrite(const Context &ctx, const UnSubscribeMessage &msg) override;

        void handleWrite(const Context &ctx, const UnSubAckMessage &msg) override;

        void handleUserMessage(const Context &ctx, const UserMessage &msg) override;
    };

}


