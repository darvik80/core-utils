//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#ifdef RASPBERRY_ARCH

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "AsyncChannel.h"

namespace network {

    template<typename Socket>
    class AsyncClient {
    public:
        typedef std::function<void(const std::shared_ptr<AsyncChannel< Socket>> &)> Callback;
    private:
        boost::asio::ssl::context _context;

        boost::asio::io_service &_service;
        Callback _callback;

        boost::asio::ip::tcp::resolver _resolver;
        boost::asio::deadline_timer _deadline;
    private:
        void doConnect(boost::asio::io_service &service, boost::asio::ip::tcp::resolver::results_type endpoints, boost::asio::ip::tcp::resolver::results_type::iterator iter) {
            auto iterNext = iter;
            iterNext++;
            if (iter != boost::asio::ip::tcp::resolver::results_type::iterator()) {
                // SSL Support
                if constexpr(std::is_base_of<SslSocket, Socket>::value) {
                    auto sslSocket = std::make_shared<Socket>(service, _context);
                    sslSocket->lowest_layer().async_connect(
                            iter->endpoint(),
                            [this, &service, endpoints, sslSocket, iterNext](const boost::system::error_code &err) {
                                if (err) {
                                    doConnect(service, endpoints, iterNext);
                                } else {
                                    sslSocket->async_handshake(
                                            boost::asio::ssl::stream_base::client,
                                            [this, &service, sslSocket, endpoints](const boost::system::error_code &ec) {
                                                if (!ec) {
                                                    std::shared_ptr<AsyncChannel<Socket>> channel(
                                                            new AsyncChannel<Socket>(std::move(*sslSocket)),
                                                            [this, &service, endpoints](AsyncChannel<Socket> *chan) {
                                                                sleep(5);
                                                                doConnect(service, endpoints, endpoints.begin());

                                                                network::log::info("destroy channel");
                                                                delete chan;
                                                            }
                                                    );
                                                    _callback(channel);
                                                    channel->start();
                                                } else {
                                                    network::log::warning("client handshake failed: {}", ec.message());
                                                    sslSocket->lowest_layer().close();
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
                            iter->endpoint(),
                            [this, &service, endpoints, socket, iterNext](const boost::system::error_code &err) {
                                if (err) {
                                    doConnect(service, endpoints, iterNext);
                                } else {
                                    std::shared_ptr<AsyncChannel<Socket>> channel(
                                            new AsyncChannel<Socket>(std::move(*socket)), [this, &service, endpoints](AsyncChannel<Socket> *chan) {
                                                sleep(5);
                                                doConnect(service, endpoints, endpoints.begin());

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
            } else {
                _deadline.expires_from_now(boost::posix_time::seconds(10));
                _deadline.async_wait([this, &service, endpoints](const boost::system::error_code &err) {
                    if (!err) {
                        doConnect(service, endpoints, endpoints.begin());
                    }
                });
            }
        }

    public:
        explicit AsyncClient(boost::asio::io_service &service, const Callback& callback)
        : _service (service), _context(boost::asio::ssl::context::tlsv12_client), _resolver(service), _deadline(service), _callback(callback) {
        }

        explicit AsyncClient(boost::asio::io_service &service, const Callback& callback, const std::string& caFile)
                : _service (service), _context(boost::asio::ssl::context::tlsv12_client), _resolver(service), _deadline(service), _callback(callback) {
            _context.use_certificate_file(caFile, boost::asio::ssl::context::pem);
            _context.set_options(boost::asio::ssl::context::default_workarounds);
        }

        void connect(std::string_view host, uint16_t port) {
            boost::asio::ip::tcp::resolver::query query(host.data(), std::to_string(port));

            auto endpoints = _resolver.resolve(query);
            doConnect(_service, endpoints, endpoints.begin());
        }

        void shutdown() {
            _deadline.cancel();
        }
    };
}

#endif