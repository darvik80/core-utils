//
// Created by Ivan Kishchenko on 13.10.2021.
//

#include <network/handler/NetworkLogger.h>
#include "AsyncTcpServer.h"

using namespace boost;

Channel::Channel(asio::ip::tcp::socket socket)
        : _socket(std::move(socket)) {
}

void Channel::start() {
    handleActive();
}

void Channel::shutdown() {
    if (_socket.is_open()) {
        handleInactive();
        network::log::info("close conn: {}:{}", _socket.remote_endpoint().address().to_string(), _socket.remote_endpoint().port());
        _socket.close();
    }
}

void Channel::doRecv() {
    auto self(shared_from_this());
    _socket.async_read_some(
            asio::buffer(_incBuf, _incBuf.size()),
            [this, self](system::error_code err, std::size_t size) {
                if (size) {
                    ByteBuf buf(_incBuf.begin(), _incBuf.begin() + size);
                    handleRead(buf);
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

void Channel::doWrite() {
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

void Channel::handleActive() {
    MessageHandler<ByteBuf, ByteBuf>::handleActive();
    doRecv();
}

void Channel::handleInactive() {
    MessageHandler<ByteBuf, ByteBuf>::handleInactive();
}

void Channel::handleError(std::error_code err) {
    network::log::info("error conn: {}:{}, {}", _socket.remote_endpoint().address().to_string(), _socket.remote_endpoint().port(), err.message());
    MessageHandler<ByteBuf, ByteBuf>::handleError(err);
}

void Channel::handleRead(const ByteBuf &event) {
    trigger(event);
}

void Channel::handleWrite(const ByteBuf &event) {
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

Channel::~Channel() {
    network::log::info("~Channel");
}

AsyncTcpServer::AsyncTcpServer(asio::io_service &service, const onConnectCallback callback)
        : _acceptor(service, asio::ip::tcp::v4()), _callback(callback) {

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
                    auto channel = std::make_shared<Channel>(std::move(socket));
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
