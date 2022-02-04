//
// Created by Ivan Kishchenko on 10.12.2021.
//

#pragma once

#include "network/Handler.h"
#include "network/Buffer.h"
#include <memory>
#include <variant>

#include "MQTT.h"
#include "MQTTWriter.h"
#include "MQTTReader.h"

namespace network::mqtt {

    class MQTTCodec : public InboundOutboundMessageHandler<Buffer, ConnectMessage, ConnAckMessage, PingReqMessage, PingRespMessage> {
        ArrayBuffer<2048> _incBuf;
    private:
        void handleReadConnect(const Context &ctx, std::istream& inc);
        void handleReadConnAck(const Context &ctx, std::istream& inc);
        void handleReadPingReq(const Context &ctx, std::istream& inc);
        void handleReadPingResp(const Context &ctx, std::istream& inc);
    public:
        void handleRead(const Context &ctx, const ByteBufferRef<uint8_t> &msg) override;

        void handleWrite(const Context &ctx, const ConnectMessage &msg) override;

        void handleWrite(const Context &ctx, const ConnAckMessage &msg) override;

        void handleWrite(const Context &ctx, const PingReqMessage &msg) override;

        void handleWrite(const Context &ctx, const PingRespMessage &msg) override;
    };

}


