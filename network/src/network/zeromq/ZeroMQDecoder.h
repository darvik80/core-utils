//
// Created by Ivan Kishchenko on 18.10.2021.
//

#pragma once

#include <memory>
#include <system_error>

#include "ZeroMQ.h"
#include "ZeroMQReader.h"

namespace network::zeromq {

    typedef std::function<void(ZeroMQMessage &msg)> ZeroMQMessageHandler;
    typedef std::function<void(ZeroMQCommand &msg)> ZeroMQCommandHandler;

    class ZeroMQDecoder {
    protected:
        ZeroMQCommandHandler _cmdHandler;
        ZeroMQMessageHandler _msgHandler;
    public:
        typedef std::shared_ptr<ZeroMQDecoder> Ptr;
        virtual std::error_code read(ByteBuffer &buf) = 0;

        void onCommand(const ZeroMQCommandHandler &handler) {
            _cmdHandler = handler;
        }

        void onMessage(const ZeroMQMessageHandler &handler) {
            _msgHandler = handler;
        }
    };

}


