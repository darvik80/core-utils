//
// Created by Ivan Kishchenko on 06.02.2022.
//

#pragma once

#include <boost/asio.hpp>
#include "network/Buffer.h"
#include "network/Handler.h"

namespace network::handler {

    class IdleStateHandler : public InboundOutboundMessageHandler<Buffer, Buffer> {
        boost::asio::deadline_timer _readTimer;
        boost::posix_time::time_duration _readTimeout;

        boost::asio::deadline_timer _writeTimer;
        boost::posix_time::time_duration _writeTimeout;

    private:
        void resetReadTimer(const Context &ctx);

        void resetWriteTimer(const Context &ctx);

    public:
        IdleStateHandler(boost::asio::io_service &service, boost::posix_time::time_duration readTimeout, boost::posix_time::time_duration writeTimeout)
                : _readTimer(service), _readTimeout(readTimeout), _writeTimer(service), _writeTimeout(writeTimeout) {}

        void handleActive(const Context &ctx) override;

        void handleRead(const Context &ctx, const Buffer &msg) override;

        void handleWrite(const Context &ctx, const Buffer &msg) override;
    };

}

