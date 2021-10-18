//
// Created by Ivan Kishchenko on 18.10.2021.
//

#pragma once

#include "network/zeromq/ZeroMQDecoder.h"

namespace network::zeromq::v31 {

    class ZeroMQDecoder : public network::zeromq::ZeroMQDecoder {
        std::unique_ptr<ZeroMQMessage> _msg;
    public:
        std::error_code read(ByteBuffer &buf) {
            ZeroMQReader reader(buf);

            if (!reader.available()) {
                return std::make_error_code(std::errc::message_size);
            }

            uint8_t flag;
            if (auto err = reader.readFlag(flag)) {
                return err;
            }

            uint64_t size{0};
            if (flag & flag_long) {
                if (auto err = reader.readSize(size)) {
                    return err;
                }
            } else {
                if (auto err = reader.readSize((uint8_t &) size)) {
                    return err;
                }
            }

            if (size > reader.available()) {
                return std::make_error_code(std::errc::message_size);
            }

            if (flag & flag_cmd) {
                ZeroMQCommand cmd;
                uint8_t nameSize;
                if (auto err = reader.readSize(nameSize)) {
                    return err;
                }

                if (auto err = reader.readString(nameSize, cmd.name)) {
                    return err;
                }
                size -= 1 + cmd.name.size();

                if (cmd.name == ZERO_MQ_CMD_READY) {
                    std::string prop, val;
                    while (size) {
                        if (auto err = reader.readSize(nameSize)) {
                            return err;
                        }

                        if (auto err = reader.readString(nameSize, prop)) {
                            return err;
                        }

                        uint32_t propSize;
                        if (auto err = reader.readSize(propSize)) {
                            return err;
                        }

                        if (auto err = reader.readString(propSize, val)) {
                            return err;
                        }
                        cmd.props.emplace(prop, val);

                        size -= (5 + prop.size() + val.size());
                    }
                } else if (cmd.name == ZERO_MQ_CMD_SUBSCRIBE || cmd.name == ZERO_MQ_CMD_CANCEL) {
                    std::string topic;
                    if (auto err = reader.readString(size, topic)) {
                        return err;
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
                if (auto err = reader.readString(size, val)) {
                    return err;
                }

                *_msg << val;

                if (!(flag & flag_more)) {
                    if (_msgHandler) {
                        _msgHandler(*_msg);
                    }
                    _msg.reset();
                }
            }

            buf.consume(buf.size() - buf.in_avail());

            return {};
        }
    };
}
