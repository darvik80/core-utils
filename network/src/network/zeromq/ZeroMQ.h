//
// Created by Ivan Kishchenko on 15.10.2021.
//

#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>

#include "network/Buffer.h"

namespace network::zeromq {

    enum ZeroMQFlag {
        flag_msg = 0x00,
        flag_cmd = 0x04,
        flag_long = 0x02,
        flag_more = 0x01,
    };

    struct ZeroMQVersion {
        uint8_t major{3};
        uint8_t minor{1};
    };

    struct ZeroMQGreeting {
        ZeroMQVersion version{};
        std::string mechanism{"NULL"};
        bool isServer{false};
    public:
        explicit ZeroMQGreeting(bool isServer)
                : mechanism("NULL"), isServer(isServer) {

        }

        ZeroMQGreeting(const ZeroMQVersion &version, const std::string &mechanism, bool isServer)
                : version(version), mechanism(mechanism), isServer(isServer) {}

        friend Writer &operator<<(Writer &out, ZeroMQGreeting &greeting) {
            out << (uint8_t) 0xFF << SetFill((uint8_t) 0x00, 8) << (uint8_t) 0x7F;
            out << (uint8_t) greeting.version.major << (uint8_t) greeting.version.minor;
            out << greeting.mechanism << SetFill((uint8_t) 0x00, 20 - greeting.mechanism.size());
            out << (uint8_t) (greeting.isServer ? 0x01 : 0x00);
            out << SetFill((uint8_t) 0x00, 31);

            return out;
        }

        friend Reader &operator>>(Reader &inc, ZeroMQGreeting &greeting) {
            if (inc.read() != 0xFF) {
                return inc;
            }
            inc << Ignore(8);
            if (inc.read() != 0x7f) {
                return inc;
            }

            inc >> greeting.version.major >> greeting.version.minor;
            greeting.mechanism.clear();
            for (auto idx = 0; idx < 20; idx++) {
                auto ch = inc.read();
                if (ch) {
                    greeting.mechanism += (char) ch;
                }
            }
            greeting.isServer = inc.read() > 0;
            inc << Ignore(31);

            return inc;
        }
    };


#define ZERO_MQ_CMD_READY           "READY"
#define ZERO_MQ_PROP_IDENTIFY       "Identify"
#define ZERO_MQ_PROP_SOCKET_TYPE    "Socket-Type"
#define ZERO_MQ_SOCKET_TYPE_PUB     "PUB"
#define ZERO_MQ_SOCKET_TYPE_SUB     "SUB"

#define ZERO_MQ_CMD_SUBSCRIBE "SUBSCRIBE"
#define ZERO_MQ_CMD_CANCEL "CANCEL"
#define ZERO_MQ_PROP_SUBSCRIPTION "Subscription"

    struct ZeroMQCommand {
        std::string name;
        std::unordered_map<std::string, std::string> props;

        explicit ZeroMQCommand(std::string_view name)
                : name(name) {}

        explicit ZeroMQCommand()
                : name{} {}

        [[nodiscard]] const std::string &getName() const {
            return name;
        }
    };

    struct ZeroMQMessage {
        std::vector<std::string> data;

        ZeroMQMessage &operator<<(std::string_view msg) {
            data.emplace_back(msg);
            return *this;
        }
    };
}
