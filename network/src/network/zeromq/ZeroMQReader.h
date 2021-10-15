//
// Created by Ivan Kishchenko on 04.09.2021.
//

#pragma once

#include <cstdint>
#include <system_error>
#include <string>
#include <vector>
#include <array>

#include <istream>

#include "network/ByteBuf.h"

namespace network::zeromq {

    class ZeroMQReader : public std::istream {
        std::size_t _readded;
    private:
        std::error_code readData(void *data, std::size_t size);

    public:
        explicit ZeroMQReader(ByteBuffer &buf);

        size_t available() {
            return rdbuf()->in_avail();
        }

        std::error_code readFlag(uint8_t &flag);

        std::error_code readSize(uint8_t &size);

        std::error_code readSize(uint32_t &size);

        std::error_code readSize(uint64_t &size);

        std::error_code readString(uint64_t size, std::string &str);

        std::error_code readBinary(uint64_t size, std::vector<uint8_t> &data);

        template<class T, std::size_t SIZE>
        std::error_code readBinary(std::array<T, SIZE> &data) {
            size_t size = SIZE * sizeof(T);
            if (size > available()) {
                return std::make_error_code(std::errc::message_size);
            }

            return readData(data.data(), size);
        }
    };

}