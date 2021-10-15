//
// Created by Ivan Kishchenko on 05.09.2021.
//

#include "ZeroMQWriter.h"
#include "ZeroMQ.h"
#include "network/Helper.h"

namespace network::zeromq {

    std::error_code ZeroMQWriter::writeData(const void *data, std::size_t size) {
        write((const char *) data, size);
        if (!(*this)) {
            return std::make_error_code(std::errc::message_size);
        }

        return {};
    }

    std::error_code ZeroMQWriter::writeFlag(uint8_t flag) {
        return writeData(&flag, sizeof(uint8_t));
    }

    std::error_code ZeroMQWriter::writeFlagAndSize(uint8_t flags, uint64_t size) {
        if (size > UINT8_MAX) {
            flags |= flag_long;
        }
        if (auto err = writeFlag(flags)) {
            return err;
        }

        if (size > UINT8_MAX) {
            if (auto err = writeSize((uint64_t) size)) {
                return err;
            }
        } else {
            if (auto err = writeSize((uint8_t) size)) {
                return err;
            }
        }

        return {};
    }

    std::error_code ZeroMQWriter::writeSize(uint8_t size) {
        return writeData(&size, sizeof(uint8_t));
    }

    std::error_code ZeroMQWriter::writeSize(uint32_t size) {
        size = Helper::swapEndian(size);
        return writeData(&size, sizeof(uint32_t));
    }

    std::error_code ZeroMQWriter::writeSize(uint64_t size) {
        size = Helper::swapEndian(size);
        return writeData(&size, sizeof(uint64_t));
    }

    std::error_code ZeroMQWriter::writeString(const std::string &str) {
        return writeData(str.data(), str.size());
    }
}