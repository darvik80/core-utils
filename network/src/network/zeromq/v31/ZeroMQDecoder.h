//
// Created by Ivan Kishchenko on 18.10.2021.
//

#pragma once

#include "network/zeromq/ZeroMQDecoder.h"

namespace network::zeromq::v31 {

    class ZeroMQDecoder : public network::zeromq::ZeroMQDecoder {
        std::unique_ptr<ZeroMQMessage> _msg;
    public:
        std::error_code read(Buffer &buf) {
            Reader reader(buf);

            if (!reader.available()) {
                return std::make_error_code(std::errc::message_size);
            }

            uint8_t flag;
            if (reader >> flag; !reader) {
                return reader.status();
            }

            uint64_t size{0};
            if (flag & flag_long) {
                reader >> size;
            } else {
                reader >> (uint8_t&)size;
            }

            if (!reader) {
                return reader.status();
            }

            if (size > reader.available()) {
                return std::make_error_code(std::errc::message_size);
            }

            if (flag & flag_cmd) {
                ZeroMQCommand cmd;

                if (reader << reader.read() >> cmd.name; !reader) {
                    return reader.status();
                }
                size -= 1 + cmd.name.size();
                if (cmd.name == ZERO_MQ_CMD_READY) {
                    std::string prop, val;
                    while (size) {
                        if (auto err = reader.read(reader.read(), prop); err) {
                            return reader.status();
                        }

                        uint32_t propSize;
                        reader.read(IOFlag::be, propSize);
                        if (auto err = reader.read(propSize, val); err) {
                            return reader.status();
                        }

                        cmd.props.emplace(prop, val);

                        size -= (1 + prop.size()) + (4 + val.size());
                    }
                } else if (cmd.name == ZERO_MQ_CMD_SUBSCRIBE || cmd.name == ZERO_MQ_CMD_CANCEL) {
                    std::string topic;
                    if (auto err = reader.read(size, topic); err) {
                        return reader.status();
                    }
                    cmd.props.emplace(ZERO_MQ_PROP_SUBSCRIPTION, topic);
                } else {
                    return std::make_error_code(std::errc::invalid_argument);
                }

                if (_cmdHandler) {
                    _cmdHandler(cmd);
                }
            } else {
                if (!_msg) {
                    _msg = std::make_unique<ZeroMQMessage>();
                }
                std::string val;
                if (reader << size >> val; !reader) {
                    return reader.status();
                }

                *_msg << val;

                if (!(flag & flag_more)) {
                    if (_msgHandler) {
                        _msgHandler(*_msg);
                    }
                    _msg.reset();
                }
            }

            buf.consume(reader.consumed());
            return {};
        }
    };
}
