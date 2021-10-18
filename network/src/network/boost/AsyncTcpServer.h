//
// Created by Ivan Kishchenko on 13.10.2021.
//

#pragma once

#ifdef RASPBERRY_ARCH

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <unordered_set>

#include "network/ByteBuf.h"
#include "network/Handler.h"
#include "AsyncChannel.h"

namespace network {

    class AsyncTcpServer {
        boost::asio::ip::tcp::acceptor _acceptor;

        onConnectCallback _callback;

        std::unordered_set<AsyncChannel*> _channels;
    private:
        void doAccept();

    public:
        explicit AsyncTcpServer(boost::asio::io_service &service, onConnectCallback callback);

        void bind(uint16_t port);

        void shutdown();
    };
}

#endif

