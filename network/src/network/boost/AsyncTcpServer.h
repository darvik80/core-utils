//
// Created by Ivan Kishchenko on 13.10.2021.
//

#pragma once

#include "network/Handler.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>


class Channel : public MessageHandler<ByteBuf, ByteBuf>, public std::enable_shared_from_this<Channel> {
    boost::asio::ip::tcp::socket _socket;
    std::array<char, 2048> _incBuf{};

    std::deque<ByteBuf> _outBufs;
private:
    void doRecv();
    void doWrite();
public:
    explicit Channel(boost::asio::ip::tcp::socket socket);

    void start();
    void shutdown() override;
    void handleActive() override;

    void handleInactive() override;

    void handleError(std::error_code err) override;

    void handleRead(const ByteBuf &event) override;

    void handleWrite(const ByteBuf &event) override;

    virtual ~Channel();
};


typedef std::function<void(const std::shared_ptr<Channel>&)> onConnectCallback;

class AsyncTcpServer {
    boost::asio::ip::tcp::acceptor _acceptor;

    onConnectCallback _callback;
private:
    void doAccept();
public:
    explicit AsyncTcpServer(boost::asio::io_service &service, onConnectCallback callback);

    void bind(uint16_t port);

    void shutdown();
};


