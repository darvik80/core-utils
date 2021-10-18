//
// Created by Ivan Kishchenko on 18.10.2021.
//

#pragma once


namespace network::zeromq::v31 {

    class ZeroMQEncoder : public network::zeromq::ZeroMQEncoder {
        std::error_code writeCmdSub(ByteBuffer &buf, const ZeroMQCommand &cmd) {
            ZeroMQWriter writer(buf);

            std::string topic = cmd.props.at(ZERO_MQ_PROP_SUBSCRIPTION);
            uint64_t expected = 1 + cmd.name.size() + topic.size();

            if (buf.available() < expected + 1) {
                return std::make_error_code(std::errc::message_size);
            }

            writer.writeFlagAndSize(flag_cmd, expected);
            writer.writeSize((uint8_t) cmd.name.size());
            writer.writeString(cmd.name);
            writer.writeString(topic);

            return {};
        }

        std::error_code writeCmdReady(ByteBuffer &buf, const ZeroMQCommand &cmd) {
            ZeroMQWriter writer(buf);
            std::size_t expected = 1 + cmd.name.size();
            for (const auto &prop: cmd.props) {
                expected += 1 + prop.first.size();
                expected += 4 + prop.second.size();
            }

            if (buf.available() < expected + 1) {
                return std::make_error_code(std::errc::message_size);
            }

            writer.writeFlagAndSize(flag_cmd, expected);
            writer.writeSize((uint8_t) cmd.name.size());
            writer.writeString(cmd.name);
            for (const auto &prop: cmd.props) {
                writer.writeSize((uint8_t) prop.first.size());
                writer.writeString(prop.first);

                writer.writeSize((uint32_t) prop.second.size());
                writer.writeString(prop.second);
            }

            return {};
        }

    public:
        std::error_code write(ByteBuffer &buf, const ZeroMQGreeting &greeting) {
            ZeroMQWriter writer(buf);
            writer << (uint8_t) 0xFF << std::setfill((char) 0x00) << std::setw(8) << (uint8_t) 0x00 << (uint8_t) 0x7F;
            writer << (uint8_t) greeting.version.major << (uint8_t) greeting.version.minor;
            writer << std::left << std::setw(20) << greeting.mechanism;
            writer << (uint8_t) (greeting.isServer ? 0x01 : 0x00);
            writer << std::setw(31) << (uint8_t) 0x00;

            return {};
        }

        std::error_code write(ByteBuffer &buf, const ZeroMQCommand &cmd) {
            if (cmd.name == ZERO_MQ_CMD_READY) {
                return writeCmdReady(buf, cmd);
            } else if (cmd.name == ZERO_MQ_CMD_SUBSCRIBE || cmd.name == ZERO_MQ_CMD_CANCEL) {
                return writeCmdSub(buf, cmd);
            }

            return {};
        }

        std::error_code write(ByteBuffer &buf, const ZeroMQMessage &msg) {
            ZeroMQWriter writer(buf);
            std::size_t expected{0};
            for (const auto &item: msg.data) {
                expected += 1 + (item.size() > UINT8_MAX ? 1 : 8) + item.size();
            }

            if (buf.available() < expected) {
                return std::make_error_code(std::errc::message_size);
            }

            for (size_t idx = 0; idx < msg.data.size(); idx++) {
                writer.writeFlagAndSize((idx < (msg.data.size() - 1) ? flag_more : flag_msg), msg.data[idx].size());
                writer.writeString(msg.data[idx]);
            }

            return {};
        }
    };

};


