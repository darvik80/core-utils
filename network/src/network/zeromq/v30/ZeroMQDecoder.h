//
// Created by Ivan Kishchenko on 18.10.2021.
//

#pragma once

#include "network/zeromq/ZeroMQDecoder.h"

namespace network::zeromq::v30 {

    class ZeroMQDecoder : public network::zeromq::ZeroMQDecoder {
        std::unique_ptr<ZeroMQMessage> _msg;
    public:
        std::error_code read(Buffer &buf) override {
            Reader reader(buf);

            if (!reader.available()) {
                return std::make_error_code(std::errc::message_size);
            }

            uint8_t flag = reader.read();

            uint64_t size{0};
            if (flag & flag_long) {
                reader >> size;
            } else {
                reader >> (uint8_t &) size;
            }

            if (!reader) {
                return reader.status();
            }

            if (size > reader.available()) {
                return std::make_error_code(std::errc::message_size);
            }

            if (flag & flag_cmd) {
                ZeroMQCommand cmd;
                if (auto err = reader.read(reader.read(), cmd.name)) {
                    return err;
                }
                size -= 1 + cmd.name.size();

                if (cmd.name == ZERO_MQ_CMD_READY) {
                    std::string prop, val;
                    while (size) {
                        if (auto err = reader.read(reader.read(), prop)) {
                            return err;
                        }

                        uint32_t propSize;
                        if (auto err = reader.read(IOFlag::be, propSize)) {
                            return err;
                        }

                        if (auto err = reader.read(propSize, val)) {
                            return err;
                        }

                        cmd.props.emplace(prop, val);

                        size -= (5 + prop.size() + val.size());
                    }
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
                if (auto err = reader.read(size, val)) {
                    return err;
                }

                *_msg << val;

                if (!(flag & flag_more)) {
                    // try detect subscribe command
                    if (_msg->data.size() == 1 && !(flag & flag_long) && _msg->data[0].size() > 1) {
                        if (_msg->data[0][0] == 0x01) {
                            ZeroMQCommand cmd{ZERO_MQ_CMD_SUBSCRIBE};
                            cmd.props.emplace(ZERO_MQ_PROP_SUBSCRIPTION, std::string(_msg->data[0].begin() + 1, _msg->data[0].end()));
                            if (_cmdHandler) {
                                _cmdHandler(cmd);
                            }
                            return {};
                        } else if (_msg->data[0][0] == 0x00) {
                            ZeroMQCommand cmd{ZERO_MQ_CMD_CANCEL};
                            cmd.props.emplace(ZERO_MQ_PROP_SUBSCRIPTION, std::string(_msg->data[0].begin() + 1, _msg->data[0].end()));
                            if (_cmdHandler) {
                                _cmdHandler(cmd);
                            }
                            return {};
                        }
                    }

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
