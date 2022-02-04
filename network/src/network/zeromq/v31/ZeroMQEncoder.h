//
// Created by Ivan Kishchenko on 18.10.2021.
//

#pragma once

#include "network/zeromq/ZeroMQEncoder.h"

namespace network::zeromq::v31 {

    class ZeroMQEncoder : public network::zeromq::ZeroMQEncoder {
        std::error_code writeCmdSub(Buffer &buf, const ZeroMQCommand &cmd) {
            Writer writer(buf);

            std::string topic = cmd.props.at(ZERO_MQ_PROP_SUBSCRIPTION);
            uint64_t expected = 1 + cmd.name.size() + topic.size();

            if (buf.available() < expected + 1) {
                return std::make_error_code(std::errc::message_size);
            }

            if (expected > UINT8_MAX) {
                writer << (uint8_t)(flag_cmd | flag_long) << expected;
            } else {
                writer << (uint8_t)(flag_cmd) << (uint8_t)expected;
            }

            writer << (uint8_t) cmd.name.size() << cmd.name << topic;

            return {};
        }

        std::error_code writeCmdReady(Buffer &buf, const ZeroMQCommand &cmd) {
            Writer writer(buf);
            uint64_t expected = 1 + cmd.name.size();
            for (const auto &prop: cmd.props) {
                expected += 1 + prop.first.size();
                expected += 4 + prop.second.size();
            }

            if (buf.available() < expected + 1) {
                return std::make_error_code(std::errc::message_size);
            }

            if (expected > UINT8_MAX) {
                writer << (uint8_t)(flag_cmd | flag_long) << expected;
            } else {
                writer << (uint8_t)(flag_cmd) << (uint8_t)expected;
            }
            writer << (uint8_t) cmd.name.size() << cmd.name;
            for (const auto &prop: cmd.props) {
                writer << (uint8_t) prop.first.size() << prop.first;
                writer << (uint32_t) prop.second.size() << prop.second;
            }

            return {};
        }

    public:
        std::error_code write(Buffer &buf, const ZeroMQCommand &cmd) {
            if (cmd.name == ZERO_MQ_CMD_READY) {
                return writeCmdReady(buf, cmd);
            } else if (cmd.name == ZERO_MQ_CMD_SUBSCRIBE || cmd.name == ZERO_MQ_CMD_CANCEL) {
                return writeCmdSub(buf, cmd);
            }

            return {};
        }

        std::error_code write(Buffer &buf, const ZeroMQMessage &msg) {
            Writer writer(buf);
            uint64_t expected{0};
            for (const auto &item: msg.data) {
                expected += 1 + (item.size() > UINT8_MAX ? 1 : 8) + item.size();
            }

            if (buf.available() < expected) {
                return std::make_error_code(std::errc::message_size);
            }

            for (size_t idx = 0; idx < msg.data.size(); idx++) {
                uint8_t  flag = (idx < (msg.data.size() - 1) ? flag_more : flag_msg);
                if (expected > UINT8_MAX) {
                    writer << (uint8_t)(flag | flag_long) << msg.data[idx].size();
                } else {
                    writer << (uint8_t)(flag) << (uint8_t)msg.data[idx].size();
                }
                writer << msg.data[idx];
            }

            return {};
        }
    };

};


