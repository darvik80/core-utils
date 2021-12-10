//
// Created by Ivan Kishchenko on 04.09.2020.
//

#ifndef MQTT_WRITER_H
#define MQTT_WRITER_H

#include <ostream>
#include <string>
#include <vector>
#include "network/Helper.h"
#include "network/ByteBuf.h"

namespace network::mqtt {

    class MQTTWriter {
    private:
        template<typename T>
        size_t writeNumber(const T &val, std::ostream &out) {
            static_assert(std::is_integral<T>::value);

            T data = network::htonT(val);
            out.write((const char *) &data, sizeof(T));

            return sizeof(T);
        };

    public:
        size_t writeInt8(int8_t val, std::ostream &out) {
            return writeNumber(val, out);
        };

        size_t writeInt16(int16_t val, std::ostream &out) {
            return writeNumber(val, out);
        };

        size_t writeInt32(int32_t val, std::ostream &out) {
            return writeNumber(val, out);
        };

        size_t writeInt64(int64_t val, std::ostream &out) {
            return writeNumber(val, out);
        };

        size_t writeUint8(uint8_t val, std::ostream &out) {
            return writeNumber(val, out);
        };

        size_t writeUint16(uint16_t val, std::ostream &out) {
            return writeNumber(val, out);
        };

        size_t writeUint32(uint32_t val, std::ostream &out) {
            return writeNumber(val, out);
        };

        size_t writeUint64(uint64_t val, std::ostream &out) {
            return writeNumber(val, out);
        };

        size_t writeString(const std::string &val, std::ostream &out) {
            size_t ret = writeUint16((uint16_t) val.size(), out);
            out.write(val.data(), val.length());

            return ret + val.length();
        };

        size_t writeData(ByteBuffer& buf, std::ostream &out) {
            out.write(buf.data(), buf.size());

            return buf.size();
        };

        size_t writeData(const std::string &val, std::ostream &out) {
            out.write(val.data(), val.length());

            return val.length();
        };

        size_t writeData(const uint8_t *val, size_t size, std::ostream &out) {
            out.write((const char *) val, size);

            return size;
        };

        size_t writeData(const std::vector<uint8_t> &val, std::ostream &out) {
            out.write((const char *) val.data(), val.size());

            return val.size();
        };

        size_t writeVariableInt(size_t val, std::ostream &inc) {
            size_t res = 0;
            do {
                uint8_t encoded = val % 0x80;
                val /= 0x80;
                if (val > 0) {
                    encoded |= 0x80;
                }

                res += writeUint8(encoded, inc);
            } while (val > 0);

            return res;
        }

    };

}

#endif //MQTT_WRITER_H
