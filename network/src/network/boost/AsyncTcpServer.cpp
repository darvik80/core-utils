//
// Created by Ivan Kishchenko on 13.10.2021.
//

#ifdef RASPBERRY_ARCH

#include <network/handler/NetworkLogger.h>

#include <utility>
#include "AsyncTcpServer.h"

namespace network {

    using namespace boost;

    AsyncTcpServer::AsyncTcpServer(asio::io_service &service, onConnectCallback  callback)
            : _acceptor(service, asio::ip::tcp::v4()), _callback(std::move(callback)) {
    }

    void AsyncTcpServer::bind(uint16_t port) {
        _acceptor.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        _acceptor.listen();
        doAccept();
    }

    void AsyncTcpServer::doAccept() {
        _acceptor.async_accept(
                [this](boost::system::error_code ec, asio::ip::tcp::socket socket) {
                    if (!ec) {
                        network::log::info("accept conn: {}:{}", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
                        auto channel = std::make_shared<AsyncChannel>(std::move(socket));
                        _callback(channel);
                        channel->start();
                    }

                    doAccept();
                }
        );
    }

    void AsyncTcpServer::shutdown() {
        _acceptor.close();
    }

}

#endif