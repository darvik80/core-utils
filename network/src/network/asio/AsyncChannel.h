//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl.hpp>
#include "network/Buffer.h"
#include "network/Handler.h"
#include <deque>

namespace network {

    typedef boost::asio::ip::tcp::socket TcpSocket;
    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SslSocket;

    template<typename Socket = TcpSocket>
    class AsyncChannel
            : public InboundOutboundMessageHandler<Buffer, Buffer>,
              public std::enable_shared_from_this<AsyncChannel<Socket>> {
        Socket _socket;

        std::vector<uint8_t> _incBuf;

        std::deque<std::vector<uint8_t>> _outBufs;

        Context _ctx;
    private:
        void doRead() {
            auto self(this->shared_from_this());
            _socket.async_read_some(
                    boost::asio::buffer(_incBuf.data(), _incBuf.size()),
                    [this, self](boost::system::error_code err, std::size_t size) {
                        if (size > 0) {
                            //network::log::debug("[net] onRead: {}:{}, capacity: {}", _ctx.address, _ctx.port, size);
                            Buffer ref(_incBuf.data(), size);
                            handleRead(_ctx, ref);
                            if (!err) {
                                doRead();
                            }
                        }

                        if (err) {
                            handleError(_ctx, err);
                            handleShutdown();
                        }
                    }
            );
        }

        void doWrite() {
            auto self(this->shared_from_this());

            _outBufs.pop_front();
            if (!_outBufs.empty()) {
                boost::asio::async_write(
                        _socket,
                        boost::asio::buffer(_outBufs.front().data(), _outBufs.front().size()),
                        [this, self](boost::system::error_code err, std::size_t size) {
                            if (!err) {
                                //network::log::debug("[net] onWrite: {}:{}, capacity: {}", _ctx.address, _ctx.port, size);
                                doWrite();
                            } else {
                                handleError(_ctx, err);
                                handleShutdown();
                            }
                        }
                );
            }
        }

    public:
        explicit AsyncChannel(Socket socket)
                : _socket(std::move(socket)) {
            _incBuf.resize(2048);

            if constexpr (std::is_base_of<SslSocket, Socket>::value) {
                _socket.set_verify_mode(boost::asio::ssl::verify_peer);
                _socket.set_verify_callback([](bool preverified, boost::asio::ssl::verify_context &ctx) -> bool {
                    // The verify callback can be used to check whether the certificate that is
                    // being presented is valid for the peer. For example, RFC 2818 describes
                    // the steps involved in doing this for HTTPS. Consult the OpenSSL
                    // documentation for more details. Note that the callback is called once
                    // for each certificate in the certificate chain, starting from the root
                    // certificate authority.

                    // In this example we will simply print the certificate's subject name.
                    char subject_name[256];
                    X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
                    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
                    network::log::info("verifying {}, {}", subject_name, preverified);

                    return preverified;
                });

                _ctx.address = _socket.lowest_layer().remote_endpoint().address().to_string();
                _ctx.port = _socket.lowest_layer().remote_endpoint().port();
                network::log::info("init ssl channel: {}:{}", _ctx.address, _ctx.port);
            }
            if constexpr (std::is_base_of<TcpSocket, Socket>::value) {
                _ctx.address = _socket.remote_endpoint().address().to_string();
                _ctx.port = _socket.remote_endpoint().port();
                network::log::info("init tcp channel: {}:{}", _ctx.address, _ctx.port);
            }
        }

        void start() {
            handleActive(_ctx);
        }

        void handleShutdown() override {
            if constexpr (std::is_base_of<SslSocket, Socket>::value) {
                if (_socket.lowest_layer().is_open()) {
                    handleInactive(_ctx);
                    _socket.lowest_layer().close();
                    //network::log::warning("[net] shutdown: {}:{}", _ctx.address, _ctx.port);
                }
            }
            if constexpr (std::is_base_of<TcpSocket, Socket>::value) {
                if (_socket.is_open()) {
                    handleInactive(_ctx);
                    _socket.close();
                    //network::log::warning("[net] shutdown: {}:{}", _ctx.address, _ctx.port);
                }
            }


        }

        void handleActive(const Context &ctx) override {
            //network::log::debug("[net] onActive: {}:{}", ctx.address, ctx.port);
            InboundMessageHandler<Buffer, Buffer>::handleActive(ctx);
            doRead();
        }

        void handleInactive(const Context &ctx) override {
            InboundMessageHandler<Buffer, Buffer>::handleInactive(ctx);
            //network::log::debug("[net] onInactive: {}:{}", ctx.address, ctx.port);
        }

        void handleRead(const Context &ctx, const Buffer &event)
        override {
            fireMessage(ctx, event);
        }

        void handleWrite(const Context &ctx, const Buffer &event)
        override {
            bool inProgress = !_outBufs.empty();
            _outBufs.emplace_back(event.data(), event.data() + event.size());
            if (!inProgress) {
                boost::asio::async_write(
                        _socket,
                        boost::asio::buffer(_outBufs.front().data(), _outBufs.front().size()),
                        [this](boost::system::error_code err, std::size_t size) {
                            if (!err) {
                                //network::log::debug("[net] onWrite: {}:{}, capacity: {}", _ctx.address, _ctx.port, size);
                                doWrite();
                            } else {
                                handleError(_ctx, err);
                                handleShutdown();
                            }
                        }
                );
            }
        }

        void handleError(const Context &ctx, std::error_code err) override {
            //network::log::warning("[net] onError: {}:{}, {}", ctx.address, ctx.port, err.message());
            InboundMessageHandler<Buffer, Buffer>::handleError(ctx, err);
        }
    };
}
