//
// Created by Ivan Kishchenko on 13.10.2021.
//

#pragma once

#include "network/Handler.h"
#include "network/ByteBuf.h"
#include <string>

namespace network::handler {

    class NetworkLogger : public MessageHandler<ByteBuffer, ByteBuffer> {
    private:
        std::string dump(ByteBuffer &buf);

    public:
        void handleRead(ByteBuffer &event) override;

        void handleWrite(ByteBuffer &event) override;
    };
}

