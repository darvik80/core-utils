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

        friend std::ostream &operator<<(std::ostream &out, const ZeroMQGreeting &greeting) {
            out << (uint8_t) 0xFF << std::setfill((char) 0x00) << std::setw(8) << (uint8_t) 0x00 << (uint8_t) 0x7F;
            out << (uint8_t) greeting.version.major << (uint8_t) greeting.version.minor;
            out << std::left << std::setw(20) << greeting.mechanism;
            out << (uint8_t) (greeting.isServer ? 0x01 : 0x00);
            out << std::setw(31) << (uint8_t) 0x00;

            return out;
        }

        friend std::istream &operator>>(std::istream &inc, ZeroMQGreeting &greeting) {
            if (inc.get() != 0xFF) {
                inc.setstate(std::ios::badbit);
                return inc;
            }
            inc.ignore(8);
            if (inc.get() != 0x7f) {
                inc.setstate(std::ios::badbit);
                return inc;
            }

            greeting.version.major = inc.get();
            greeting.version.minor = inc.get();
            for (auto idx = 0; idx < 20; idx++) {
                auto ch = inc.get();
                if (ch) {
                    greeting.mechanism += (char) ch;
                }
            }
            greeting.isServer = inc.get() > 0;
            inc.ignore(31);
            if (inc.eof()) {
                return inc;

            }

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

    typedef std::variant<ZeroMQCommand, ZeroMQMessage> ZeroMQMsg;
}
