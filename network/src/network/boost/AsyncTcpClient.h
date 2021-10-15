//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "AsyncChannel.h"

namespace network {

    class AsyncTcpClient {
        boost::asio::io_service &_service;
        onConnectCallback _callback;

        boost::asio::ip::tcp::resolver _resolver;
        boost::asio::deadline_timer _deadline;
    private:
        void doConnect(boost::asio::io_service &service, boost::asio::ip::tcp::resolver::results_type endpoints, boost::asio::ip::tcp::resolver::results_type::iterator iter);
    public:
        explicit AsyncTcpClient(boost::asio::io_service &service, const onConnectCallback& callback);

        void connect(std::string_view host, uint16_t port);

        void shutdown();
    };

}

