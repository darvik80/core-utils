//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#ifdef RASPBERRY_ARCH

#include <boost/asio.hpp>
#include "network/ByteBuf.h"
#include "network/Handler.h"
#include <deque>

namespace network {

    class AsyncChannel : public InboundOutboundMessageHandler<ByteBufferRef<uint8_t>, ByteBufferRef<uint8_t>>, public std::enable_shared_from_this<AsyncChannel> {
        boost::asio::ip::tcp::socket _socket;

        std::vector<uint8_t> _incBuf;

        std::deque<std::vector<uint8_t>> _outBufs;

        Context _ctx;
    private:
        void doRead();

        void doWrite();

    public:
        explicit AsyncChannel(boost::asio::ip::tcp::socket socket);

        void start();

        void handleShutdown() override;

        void handleActive(const Context &ctx) override;

        void handleInactive(const Context &ctx) override;

        void handleRead(const Context &ctx, const ByteBufferRef<uint8_t> &event) override;

        void handleWrite(const Context &ctx, const ByteBufferRef<uint8_t> &event) override;

        void handleError(const Context &ctx, std::error_code err) override;

        void shutdown();
        ~AsyncChannel() override;
    };

    typedef std::function<void(const std::shared_ptr<AsyncChannel> &)> onConnectCallback;

}

#endif