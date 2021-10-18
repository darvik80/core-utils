//
// Created by Ivan Kishchenko on 18.10.2021.
//

#pragma once

#include <memory>
#include <system_error>
#include <functional>

#include "ZeroMQ.h"
#include "ZeroMQWriter.h"

namespace network::zeromq {

    class ZeroMQEncoder {
    public:
        typedef std::shared_ptr<ZeroMQEncoder> Ptr;
        virtual std::error_code write(ByteBuffer &buf, const ZeroMQGreeting &greeting) = 0;
        virtual std::error_code write(ByteBuffer &buf, const ZeroMQCommand &cmd) = 0;
        virtual std::error_code write(ByteBuffer &buf, const ZeroMQMessage &msg) = 0;
    };
}


