//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include "network/Handler.h"
#include "network/Buffer.h"
#include "ZeroMQDecoder.h"
#include "ZeroMQEncoder.h"

#include <memory>
#include <variant>

namespace network::zeromq {

    enum class ZeroMQState {
        Greeting,
        Stream,
    };

    class ZeroMQCodec : public InboundOutboundMessageHandler<Buffer, ZeroMQCommand, ZeroMQMessage> {
        ZeroMQState _state{ZeroMQState::Greeting};

        ArrayBuffer<2048> _incBuf;
        ZeroMQDecoder::Ptr _decoder;
        ZeroMQEncoder::Ptr _encoder;
    public:
        void handleActive(const Context &ctx) override;

        void handleRead(const Context &ctx, const Buffer &event) override;

        void handleWrite(const Context &ctx, const ZeroMQCommand &msg) override;

        void handleWrite(const Context &ctx, const ZeroMQMessage &msg) override;
    };
}


