//
// Created by Ivan Kishchenko on 10.12.2021.
//

#include "MQTTCodec.h"
#include "network/mqtt/v31/MQTTDecoder.h"
#include "network/mqtt/v31/MQTTEncoder.h"
#include "MQTTLogging.h"

namespace network::mqtt {
    MQTTCodec::MQTTCodec(const MQTTOptions options)
            : _options(options) {
        _decoder = std::make_shared<v31::MQTTDecoder>();
        _encoder = std::make_shared<v31::MQTTEncoder>();
    }


    void MQTTCodec::handleRead(const Context &ctx, const Buffer &msg) {
        _incBuf.append(msg.data(), msg.size());
        _decoder->read(_incBuf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const PublishMessage &msg) {
        mqtt::log::debug("write Pub");
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const PubAckMessage &msg) {
        mqtt::log::debug("write PubAck");
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const SubscribeMessage &msg) {
        mqtt::log::debug("write Sub");
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const SubAckMessage &msg) {
        mqtt::log::debug("write SubAck");
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const UnSubscribeMessage &msg) {
        mqtt::log::debug("write UnSub");
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const UnSubAckMessage &msg) {
        mqtt::log::debug("write UnSubAck");
        ArrayBuffer<1024> buf;
        _encoder->write(buf, msg);
        write(ctx, buf);
    }


    void MQTTCodec::handleActive(const Context &ctx) {
        ArrayBuffer<128> buf;
        ConnectMessage msg;
        msg.setClientId(_options.clientId);
        if (!_options.accessToken.empty()) {
            msg.setUserName(_options.accessToken);
        } else {
            msg.setUserName(_options.username);
            msg.setPassword(_options.password);
        }
        _encoder->write(buf, msg);
        write(ctx, buf);

        _decoder->onConnAck([this, ctx](const ConnAckMessage &msg) {
            mqtt::log::info("handle ConnAck: {}:{}", msg.getReasonCode(), msg.getReasonCodeDescription());
            if (msg.getReasonCode()) {
                fireShutdown();
            } else {
                InboundMessageHandler::handleActive(ctx);
            }
        });

        _decoder->onPubAck([](const PubAckMessage &msg) {
            mqtt::log::debug("handle PubAck: {}", msg.getPacketIdentifier());
        });

        _decoder->onPong([](const PingRespMessage &msg) {
            mqtt::log::debug("handle PingResp");
        });

        _decoder->onSubAck([](const SubAckMessage &msg) {
            mqtt::log::debug("handle SubAck: {}", msg.getPacketIdentifier(), msg.getReturnCode());
        });

        _decoder->onUnSubAck([](const UnSubAckMessage &msg) {
            mqtt::log::debug("handle UnSubAck: {}", msg.getPacketIdentifier());
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
