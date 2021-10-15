//
// Created by Ivan Kishchenko on 15.10.2021.
//

#ifdef RASPBERRY_ARCH

#include "AsyncChannel.h"

namespace network{

    using namespace boost;

    AsyncChannel::AsyncChannel(asio::ip::tcp::socket socket)
            : _socket(std::move(socket)) {
    }

    void AsyncChannel::start() {
        handleActive();
    }

    void AsyncChannel::shutdown() {
        if (_socket.is_open()) {
            handleInactive();
            network::log::info("close conn");
            _socket.close();
        }
    }

    void AsyncChannel::doRecv() {
        auto self(shared_from_this());
        _socket.async_read_some(
                asio::buffer(_inc.data(), _inc.size()),
                [this, self](system::error_code err, std::size_t size) {
                    if (size > 0) {
                        _incBuf.append(_inc.data(), (std::streamsize)size);
                        handleRead(_incBuf);
                        if (!err) {
                            doRecv();
                        }
                    }

                    if (err) {
                        handleError(err);
                        shutdown();
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
                    [this, self](boost::system::error_code err, std::size_t) {
                        if (!err) {
                            doWrite();
                        } else {
                            handleError(err);
                            shutdown();
                        }
                    }
            );
        }
    }

    void AsyncChannel::handleActive() {
        MessageHandler<ByteBuffer, ByteBuffer>::handleActive();
        doRecv();
    }

    void AsyncChannel::handleInactive() {
        MessageHandler<ByteBuffer, ByteBuffer>::handleInactive();
    }

    void AsyncChannel::handleError(std::error_code err) {
        network::log::info("error conn: {}", err.message());
        MessageHandler<ByteBuffer , ByteBuffer>::handleError(err);
    }

    void AsyncChannel::handleRead(ByteBuffer &event) {
        trigger(event);
    }

    void AsyncChannel::handleWrite(ByteBuffer &event) {
        bool inProgress = !_outBufs.empty();
        _outBufs.push_back(event);
        if (!inProgress) {
            boost::asio::async_write(
                    _socket,
                    boost::asio::buffer(_outBufs.front().data(), _outBufs.front().size()),
                    [this](boost::system::error_code err, std::size_t) {
                        if (!err) {
                            doWrite();
                        } else {
                            handleError(err);
                            shutdown();
                        }
                    }
            );
        }
    }

    AsyncChannel::~AsyncChannel() {
        network::log::info("~Channel");
    }
}

#endif