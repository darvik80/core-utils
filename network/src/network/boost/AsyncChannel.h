//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include <boost/asio.hpp>
#include "network/ByteBuf.h"
#include "network/Handler.h"

namespace network {

    class AsyncChannel : public MessageHandler<ByteBuffer, ByteBuffer>, public std::enable_shared_from_this<AsyncChannel> {
        boost::asio::ip::tcp::socket _socket;
        std::array<char, 1024> _inc{};
        ByteBufFix<2048> _incBuf;

        std::deque <ByteBuffer> _outBufs;
    private:
        void doRecv();

        void doWrite();

    public:
        explicit AsyncChannel(boost::asio::ip::tcp::socket socket);

        void start();

        void shutdown() override;

        void handleActive() override;

        void handleInactive() override;

        void handleError(std::error_code err) override;

        void handleRead(ByteBuffer &event) override;

        void handleWrite(ByteBuffer &event) override;

        ~AsyncChannel() override;
    };

    typedef std::function<void(const std::shared_ptr<AsyncChannel> &)> onConnectCallback;

}
