//
// Created by Ivan Kishchenko on 15.10.2021.
//

#ifdef RASPBERRY_ARCH

#include "AsyncChannel.h"

namespace network {

    using namespace boost;

    AsyncChannel::AsyncChannel(asio::ip::tcp::socket socket)
            : _socket(std::move(socket)) {
        _ctx.address = _socket.remote_endpoint().address().to_string();
        _ctx.port = _socket.remote_endpoint().port();
        _incBuf.resize(2048);
    }

    void AsyncChannel::start() {
        handleActive(_ctx);
    }


    void AsyncChannel::handleShutdown() {
        if (_socket.is_open()) {
            handleInactive(_ctx);
            _socket.close();
            network::log::warning("[net] shutdown: {}:{}", _ctx.address, _ctx.port);
        }
    }

    void AsyncChannel::doRead() {
        auto self(shared_from_this());
        _socket.async_read_some(
                asio::buffer(_incBuf.data(), _incBuf.size()),
                [this, self](system::error_code err, std::size_t size) {
                    if (size > 0) {
                        network::log::debug("[net] onRead: {}:{}, capacity: {}", _ctx.address, _ctx.port, size);
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

    void AsyncChannel::doWrite() {
        auto self(shared_from_this());

        _outBufs.pop_front();
        if (!_outBufs.empty()) {
            asio::async_write(
                    _socket,
                    asio::buffer(_outBufs.front().data(), _outBufs.front().size()),
                    [this, self](boost::system::error_code err, std::size_t size) {
                        if (!err) {
                            network::log::debug("[net] onWrite: {}:{}, capacity: {}", _ctx.address, _ctx.port, size);
                            doWrite();
                        } else {
                            handleError(_ctx, err);
                            handleShutdown();
                        }
                    }
            );
        }
    }

    void AsyncChannel::handleActive(const Context &ctx) {
        network::log::debug("[net] onActive: {}:{}", ctx.address, ctx.port);
        InboundMessageHandler<Buffer, Buffer>::handleActive(ctx);
        doRead();
    }

    void AsyncChannel::handleInactive(const Context &ctx) {
        InboundMessageHandler<Buffer, Buffer>::handleInactive(ctx);
        network::log::debug("[net] onInactive: {}:{}", ctx.address, ctx.port);
    }

    void AsyncChannel::handleError(const Context &ctx, std::error_code err) {
        network::log::warning("[net] onError: {}:{}, {}", ctx.address, ctx.port, err.message());
        InboundMessageHandler<Buffer, Buffer>::handleError(ctx, err);
    }

    void AsyncChannel::handleRead(const Context &ctx, const Buffer &event) {
        fireMessage(ctx, event);
    }

    void AsyncChannel::handleWrite(const Context &ctx, const Buffer &event) {
        bool inProgress = !_outBufs.empty();
        _outBufs.emplace_back(event.data(), event.data() + event.size());
        if (!inProgress) {
            boost::asio::async_write(
                    _socket,
                    boost::asio::buffer(_outBufs.front().data(), _outBufs.front().size()),
                    [this](boost::system::error_code err, std::size_t size) {
                        if (!err) {
                            network::log::debug("[net] onWrite: {}:{}, capacity: {}", _ctx.address, _ctx.port, size);
                            doWrite();
                        } else {
                            handleError(_ctx, err);
                            handleShutdown();
                        }
                    }
            );
        }
    }

    AsyncChannel::~AsyncChannel() {
        network::log::debug("~Channel");
    }

}

#endif