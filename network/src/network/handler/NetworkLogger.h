//
// Created by Ivan Kishchenko on 13.10.2021.
//

#pragma once

#include "network/Handler.h"
#include "network/Buffer.h"
#include <string>

namespace network::handler {

    class NetworkLogger : public InboundOutboundMessageHandler<Buffer, Buffer> {
    private:
        std::string dump(const Buffer &buf);

    public:
        void handleActive(const Context &ctx) override;

        void handleInactive(const Context &ctx) override;

        void handleError(const Context &ctx, std::error_code err) override;

        void handleRead(const Context &ctx, const Buffer &event) override;

        void handleWrite(const Context &ctx, const Buffer &msg) override;
    };
}

