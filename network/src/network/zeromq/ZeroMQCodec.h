//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include "network/Handler.h"
#include "network/ByteBuf.h"
#include "ZeroMQDecoder.h"
#include "ZeroMQEncoder.h"

#include <memory>
#include <variant>

namespace network::zeromq {

    enum class ZeroMQState {
        Greeting,
        Stream,
    };

    class ZeroMQCodec : public InboundOutboundMessageHandler<ByteBufferRef<uint8_t>, ZeroMQCommand, ZeroMQMessage> {
        ZeroMQState _state{ZeroMQState::Greeting};

        ByteBufFix<2048> _incBuf;
        ZeroMQDecoder::Ptr _decoder;
        ZeroMQEncoder::Ptr _encoder;
    public:
        void handleActive(const Context &ctx) override;

        void handleRead(const Context &ctx, const ByteBufferRef<uint8_t> &event) override;

        void handleWrite(const Context &ctx, const ZeroMQCommand &msg) override;

        void handleWrite(const Context &ctx, const ZeroMQMessage &msg) override;
    };
}


