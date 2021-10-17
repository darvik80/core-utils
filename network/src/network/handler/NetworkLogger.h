//
// Created by Ivan Kishchenko on 13.10.2021.
//

#pragma once

#include "network/Handler.h"
#include "network/ByteBuf.h"
#include <string>

namespace network::handler {

    class NetworkLogger : public InboundOutboundMessageHandler<ByteBufferRef<uint8_t>, ByteBufferRef<uint8_t>> {
    private:
        std::string dump(const ByteBufferRef<uint8_t> &buf);

    public:
        void handleActive(const Context &ctx) override;

        void handleInactive(const Context &ctx) override;

        void handleError(const Context &ctx, std::error_code err) override;

        void handleRead(const Context &ctx, const ByteBufferRef<uint8_t> &event) override;

        void handleWrite(const Context &ctx, const ByteBufferRef<uint8_t> &msg) override;
    };
}

