//
// Created by Ivan Kishchenko on 10.12.2021.
//

#include "MQTTCodec.h"
#include "network/mqtt/v31/MQTTDecoder.h"
#include "network/mqtt/v31/MQTTEncoder.h"
#include "MQTTLogging.h"

namespace network::mqtt {
    MQTTCodec::MQTTCodec() {
        _decoder = std::make_shared<v31::MQTTDecoder>();
        _encoder = std::make_shared<v31::MQTTEncoder>();
    }


    void MQTTCodec::handleRead(const network::Context &ctx, const network::Buffer &msg) {
        _incBuf.append(msg.data(), msg.size());
        _decoder->read(_incBuf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const PublishMessage &msg) {
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const PubAckMessage &msg) {
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const SubscribeMessage &msg) {
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const SubAckMessage &msg) {
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }

    void MQTTCodec::handleActive(const Context &ctx) {
        //InboundMessageHandler::handleActive(ctx);
        ArrayBuffer<128> buf;
        ConnectMessage msg;
        msg.setClientId("mqtt-tester");
        msg.setUserName("hello-world");
        _encoder->write(buf, msg);
        write(ctx, buf);

        _decoder->onConnAck([this](const ConnAckMessage &msg) {
            mqtt::log::info("handle ConnAck: {}:{}", msg.getReasonCode(), msg.getReasonCodeDescription());
            if (msg.getReasonCode()) {
                fireShutdown();
            }
        });

        _decoder->onPubAck([](const PubAckMessage &msg) {
            mqtt::log::debug("handle PubAck: {}", msg.getPacketIdentifier());
        });

        _decoder->onPong([](const PingRespMessage &msg) {
            mqtt::log::debug("handle PingResp");
        });
    }

    void MQTTCodec::handleUserMessage(const Context &ctx, const UserMessage &msg) {
        if (msg.getId() == read_idle) {
            mqtt::log::info("handle read timeout idle: {}", msg.getId());
            ArrayBuffer<8> buf;
            PingReqMessage ping;
            _encoder->write(buf, ping);
            write(ctx, buf);
        }

        InboundMessageHandler::handleUserMessage(ctx, msg);
    }
}
