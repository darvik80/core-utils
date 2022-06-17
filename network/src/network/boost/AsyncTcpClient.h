//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "AsyncChannel.h"
#include "DnsResolver.h"

namespace network {

    template<typename Socket>
    class AsyncClient {
    public:
        typedef std::function<void(const std::shared_ptr<AsyncChannel < Socket>> &)>
        Callback;
    private:
        boost::asio::ssl::context _context;

        boost::asio::io_service &_service;
        Callback _callback;

        DnsResolver _resolver;
        boost::asio::deadline_timer _deadline;
    private:
        void doReconnect(boost::asio::io_service &service) {
            _deadline.expires_from_now(boost::posix_time::seconds(10));
            _deadline.async_wait([this, &service](const boost::system::error_code &err) {
                if (!err) {
                    doConnect(service);
                }
            });
        }
        void doConnect(boost::asio::io_service &service) {
            if constexpr(std::is_base_of<SslSocket, Socket>::value) {
                auto sslSocket = std::make_shared<Socket>(service, _context);
                sslSocket->lowest_layer().async_connect(
                        _resolver.next(),
                        [this, &service, sslSocket](const boost::system::error_code &err) {
                            if (err) {
                                doConnect(service);
                            } else {
                                sslSocket->async_handshake(
                                        boost::asio::ssl::stream_base::client,
                                        [this, &service, sslSocket](const boost::system::error_code &ec) {
                                            if (!ec) {
                                                std::shared_ptr<AsyncChannel<Socket>> channel(
                                                        new AsyncChannel<Socket>(std::move(*sslSocket)),
                                                        [this, &service](AsyncChannel<Socket> *chan) {
                                                            doReconnect(service);
                                                            network::log::info("destroy channel");
                                                            delete chan;
                                                        }
                                                );
                                                _callback(channel);
                                                channel->start();
                                            } else {
                                                network::log::warning("client handshake failed: {}", ec.message());
                                                sslSocket->lowest_layer().close();
                                                doReconnect(service);
                                            }
                                        }
                                );
                            }
                        }
                );
            }
            // TCP Support
            if constexpr(std::is_base_of<TcpSocket, Socket>::value) {
                auto socket = std::make_shared<Socket>(service);
                socket->async_connect(
                        _resolver.next(),
                        [this, &service, socket](const boost::system::error_code &err) {
                            if (err) {
                                doConnect(service);
                            } else {
                                std::shared_ptr<AsyncChannel<Socket>> channel(
                                        new AsyncChannel<Socket>(std::move(*socket)), [this, &service](AsyncChannel<Socket> *chan) {
                                            doReconnect(service);
                                            network::log::info("destroy channel");
                                            delete chan;
                                        }
                                );
                                _callback(channel);
                                channel->start();
                            }
                        }
                );
            }
        }

    public:
        explicit AsyncClient(boost::asio::io_service &service, const Callback &callback)
                : _service(service), _resolver(service), _context(boost::asio::ssl::context::tlsv12_client), _deadline(service), _callback(callback) {
        }

        explicit AsyncClient(boost::asio::io_service &service, const Callback &callback, const std::string &caFile)
                : _service(service), _resolver(service), _context(boost::asio::ssl::context::tlsv12_client), _deadline(service), _callback(callback) {
            _context.use_certificate_file(caFile, boost::asio::ssl::context::pem);
            _context.set_options(boost::asio::ssl::context::default_workarounds);
        }

        void connect(std::string_view host, uint16_t port) {
            boost::asio::ip::tcp::resolver::query query(host.data(), std::to_string(port));

            _resolver.resolve(host, port);
            doConnect(_service);
        }

        void shutdown() {
            _deadline.cancel();
        }
    };
}
