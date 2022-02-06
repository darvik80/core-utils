//
// Created by Ivan Kishchenko on 13.10.2021.
//

#pragma once

#ifdef RASPBERRY_ARCH

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <unordered_set>

#include "network/Handler.h"
#include "AsyncChannel.h"

namespace network {

    template<typename Socket>
    class AsyncServer {
    public:
        typedef std::function<void(const std::shared_ptr<AsyncChannel<Socket>> &)> Callback;
    private:
        boost::asio::ssl::context _context;
        boost::asio::ip::tcp::acceptor _acceptor;

        Callback _callback;

        std::unordered_set<AsyncChannel<Socket> *> _channels;
    private:
        void doAccept() {
            _acceptor.async_accept(
                    [this](const boost::system::error_code &ec, boost::asio::ip::tcp::socket socket) {
                        if (!ec) {
                            network::log::debug("[net] onAccept: {}:{}", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());

                            // SSL Support
                            if constexpr(std::is_base_of<SslSocket, Socket>::value) {
                                auto sslSocket = std::make_shared<Socket>(std::move(socket), _context);
                                sslSocket->async_handshake(
                                        boost::asio::ssl::stream_base::server,
                                        [this, sslSocket](const boost::system::error_code &ec) {
                                            if (!ec) {
                                                std::shared_ptr<AsyncChannel<SslSocket>> channel(new AsyncChannel<Socket>(std::move(*sslSocket)), [this](AsyncChannel<Socket> *chan) {
                                                    _channels.erase(chan);
                                                    network::log::info("[net] destroy channel");
                                                    delete chan;
                                                });
                                                _channels.insert(channel.get());

                                                _callback(channel);
                                                channel->start();
                                            } else {
                                                network::log::warning("server handshake failed: {}", ec.message());
                                                sslSocket->lowest_layer().close();
                                            }
                                        }
                                );
                            }

                            if constexpr(std::is_base_of<TcpSocket, Socket>::value) {
                                std::shared_ptr<AsyncChannel<Socket>> channel(new AsyncChannel(std::move(socket)), [this](AsyncChannel<Socket> *chan) {
                                    _channels.erase(chan);
                                    network::log::info("[net] destroy channel");
                                    delete chan;
                                });
                                _channels.insert(channel.get());

                                _callback(channel);
                                channel->start();
                            }


                        }

                        doAccept();
                    }
            );
        }

    public:
        explicit AsyncServer(boost::asio::io_service &service, Callback callback)
                : _acceptor(service, boost::asio::ip::tcp::v4()), _context(boost::asio::ssl::context::tlsv12_server), _callback(std::move(callback)) {
        }

        explicit AsyncServer(boost::asio::io_service &service, Callback callback, const std::string &certFile, const std::string &keyFile)
                : _acceptor(service, boost::asio::ip::tcp::v4()), _context(boost::asio::ssl::context::tlsv12_server), _callback(std::move(callback)) {
            _context.use_certificate_file(certFile, boost::asio::ssl::context::pem);
            _context.use_private_key_file(keyFile, boost::asio::ssl::context::pem);
            _context.set_options(boost::asio::ssl::context::default_workarounds);
        }

        void bind(uint16_t port) {
            _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            _acceptor.set_option(boost::asio::ip::tcp::acceptor::enable_connection_aborted(true));
            _acceptor.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
            _acceptor.listen();
            doAccept();
            network::log::info("[net] start tcp server: {}", port);
        }

        void shutdown() {
            _acceptor.close();
            network::log::info("[net] shutdown tcp server");
            for (auto &channel: _channels) {
                network::log::info("[net] shutdown tcp client");
                channel->handleShutdown();
            }
        }
    };
}

#endif

