//
// Created by Ivan Kishchenko on 05.09.2021.
//

#pragma once

#include <cstdint>
#include <system_error>
#include <ostream>
#include "network/ByteBuf.h"

namespace network::zeromq {

    class ZeroMQWriter : public std::ostream {
    private:
        std::error_code writeData(const void *data, std::size_t size);

    public:
        explicit ZeroMQWriter(ByteBuffer &buf)
                : std::ostream(&buf) {}

        std::error_code writeFlagAndSize(uint8_t flags, uint64_t size);

        std::error_code writeFlag(uint8_t flag);

        std::error_code writeSize(uint8_t size);

        std::error_code writeSize(uint32_t size);

        std::error_code writeSize(uint64_t size);

        std::error_code writeString(const std::string &str);

    };

}