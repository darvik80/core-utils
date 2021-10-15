//
// Created by Ivan Kishchenko on 04.09.2021.
//

#include "ZeroMQReader.h"
#include "network/Helper.h"

namespace network::zeromq {

    ZeroMQReader::ZeroMQReader(ByteBuffer &buf)
            : std::istream(&buf), _readded(0) {}

    std::error_code ZeroMQReader::readFlag(uint8_t &flag) {
        return readData(&flag, sizeof(uint8_t));
    }

    std::error_code ZeroMQReader::readSize(uint8_t &size) {
        return readData(&size, sizeof(uint8_t));
    }

    std::error_code ZeroMQReader::readSize(uint64_t &val) {
        if (auto err = readData(&val, sizeof(uint64_t))) {
            return err;
        }

        Helper::swapEndian(val);

        return {};
    }

    std::error_code ZeroMQReader::readSize(uint32_t &val) {
        if (auto err = readData(&val, sizeof(uint32_t))) {
            return err;
        }

        val = Helper::swapEndian(val);

        return {};
    }

    std::error_code ZeroMQReader::readString(uint64_t size, std::string &str) {
        if (size > available()) {
            return std::make_error_code(std::errc::message_size);
        }
        str.clear();
        uint8_t ch;
        for (std::size_t idx = 0; idx < size; idx++) {
            if (auto err = readData(&ch, sizeof(uint8_t))) {
                return err;
            }

            str += (char) ch;
        }

        return {};
    }

    std::error_code ZeroMQReader::readBinary(uint64_t size, std::vector<uint8_t> &vec) {
        if (size > available()) {
            return std::make_error_code(std::errc::message_size);
        }

        vec.resize(size);

        return readData(&vec.front(), size);
    }

    std::error_code ZeroMQReader::readData(void *data, std::size_t size) {
        if (size > available()) {
            return std::make_error_code(std::errc::message_size);
        }

        read((char *) data, size);

        _readded += size;

        return {};
    }

}