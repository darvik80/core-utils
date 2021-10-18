//
// Created by Ivan Kishchenko on 13.10.2021.
//

#ifdef RASPBERRY_ARCH

#include <network/handler/NetworkLogger.h>

#include <utility>
#include "AsyncTcpServer.h"

namespace network {

    using namespace boost;

    AsyncTcpServer::AsyncTcpServer(asio::io_service &service, onConnectCallback callback)
            : _acceptor(service, asio::ip::tcp::v4()), _callback(std::move(callback)) {
    }

    void AsyncTcpServer::bind(uint16_t port) {
        _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        _acceptor.set_option(asio::ip::tcp::acceptor::enable_connection_aborted(true));
        _acceptor.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        _acceptor.listen();
        doAccept();
        network::log::info("[net] start tcp server: {}", port);
    }

    void AsyncTcpServer::doAccept() {
        _acceptor.async_accept(
                [this](boost::system::error_code ec, asio::ip::tcp::socket socket) {
                    if (!ec) {
                        network::log::debug("[net] onAccept: {}:{}", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
                        std::shared_ptr<AsyncChannel> channel(new AsyncChannel(std::move(socket)), [this](AsyncChannel *chan) {
                            _channels.erase(chan);
                            network::log::info("[net] destroy channel");
                            delete chan;
                        });
                        _channels.insert(channel.get());

                        _callback(channel);
                        channel->start();
                    }

                    doAccept();
                }
        );
    }

    void AsyncTcpServer::shutdown() {
        _acceptor.close();
        network::log::info("[net] shutdown tcp server");
        for (auto& channel : _channels) {
            network::log::info("[net] shutdown tcp client");
            channel->handleShutdown();
        }
    }
}

#endif