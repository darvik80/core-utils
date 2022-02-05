//
// Created by Ivan Kishchenko on 05.02.2022.
//

#pragma once

#include "network/mqtt/MQTTEncoder.h"

namespace network::mqtt::v31 {

    class MQTTEncoder : public network::mqtt::MQTTEncoder {
    public:
        std::error_code write(Buffer &buf, const ConnectMessage &msg) override  {
            VectorBuffer data;
            Writer writer(data);

            /// 3.1.2.1 Protocol name
            writer << IOFlag::be << (uint16_t)msg.getProtocolName().size() << msg.getProtocolName();
            /// 3.1.2.2 Protocol version
            writer << msg.getProtocolLevel();

            /// 3.1.2.2 Protocol flags
            writer << msg.getFlags().all;
            /// 3.1.2.10 Keep alive
            writer << msg.getKeepAlive();

            /// 3.1.3 Payload
            /// 3.1.3.1 DefaultConnection Id
            writer << IOFlag::be << (uint16_t)msg.getClientId().size() << msg.getClientId();

            if (msg.getFlags().bits.willFlag) {
                /// 3.1.3.2 Will Topic
                writer << IOFlag::be << (uint16_t)msg.getWillTopic().size() << msg.getWillTopic();
                /// 3.1.3.3 Will Message
                writer << IOFlag::be << (uint16_t)msg.getWillMessage().size() << msg.getWillMessage();
            }

            if (msg.getFlags().bits.username) {
                /// 3.1.3.4 User Name
                writer << IOFlag::be << (uint16_t)msg.getUserName().size() << msg.getUserName();
            }

            if (msg.getFlags().bits.password) {
                /// 3.1.3.5 Password
                writer << IOFlag::be << (uint16_t)msg.getPassword().size() << msg.getPassword();
            }
            if (!writer) {
                return writer.status();
            }

            Writer out(buf);
            out << msg.getHeader().all << IOFlag::variable << data.size() << data;

            return out.status();
        }

        std::error_code write(Buffer &buf, const ConnAckMessage &msg) override  {
            Writer out(buf);
            out << msg.getHeader().all << IOFlag::variable << 2;

            /// 3.2.2 Variable header
            /// 3.2.2.1 Connect Acknowledge Flags
            out << msg.getFlags();

            /// 3.2.2.2 Session Present
            /// 3.2.2.3 Connect Return code
            out << msg.getReasonCode();

            return out.status();
        }

        std::error_code write(Buffer &buf, const PingReqMessage &msg) override {
            Writer out(buf);

            out << msg.getHeader().all << IOFlag::variable << 0;

            return out.status();
        }

        std::error_code write(Buffer &buf, const PingRespMessage &msg) override {
            Writer out(buf);

            out << msg.getHeader().all << IOFlag::variable << 0;

            return out.status();
        }

    };
}