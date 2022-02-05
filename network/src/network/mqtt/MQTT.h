//
// Created by Ivan Kishchenko on 10.12.2021.
//

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include "network/Buffer.h"

namespace network::mqtt {

    enum class MessageType {
        reserved,
        connect,        // К* -> С**	    connection requests
        conn_ack,       // К <- С	        connection confirmed
        publish,        // К <- С, К -> С   publish msg
        pub_ack,        // К <- С, К -> С   publish confirmed
        pub_rec,        // К <- С, К -> С   publish received
        pub_rel,        // К <- С, К -> С   msg can be deleted
        pub_comp,       // К <- С, К -> С   publish finished
        subscribe,      // К -> С           request for subscribe
        sub_ack,        // К <- С           subscribe confirmed
        unsubscribe,    // К -> С           request for unsubscribe
        unsub_ack,      // К <- С           unsubscribe confirmed
        ping_req,       // К -> С           ping
        ping_resp,      // К <- С           pong
        disconnect,     // К -> С           disconnect
        reserved_done
    };

    union Header {
        uint8_t all;
        struct {
            bool retain: 1;
            unsigned int qos: 2;    /**< QoS value, 0, 1 or 2 */
            bool dup: 1;            /**< DUP flag bit */
            unsigned int type: 4;    /**< message type nibble */
        } bits;
    };

    typedef union {
        uint8_t all;
        struct {
            int reserved: 1;                    /**< unused */
            bool cleanSession: 1;        /**< cleansession flag */
            bool willFlag: 1;              /**< will flag */
            unsigned int willQoS: 2;   /**< will QoS value */
            bool willRetain: 1;        /**< will retain setting */
            bool password: 1;          /**< 3.1 password */
            bool username: 1;          /**< 3.1 user name */
        } bits;
    } Flags;

    struct Message {
        Header _header{};
        varInt _size{};
    public:
        explicit Message(MessageType type) {
            _header.bits.type = (unsigned)type;
        }

        void setQos(uint8_t qos) {
            _header.bits.qos = qos;
        }

        [[nodiscard]] uint8_t getQos() const {
            return _header.bits.qos;
        }

        void setRetain(bool retain) {
            _header.bits.retain = retain;
        }

        [[nodiscard]] virtual uint8_t getType() const {
            return _header.bits.type;
        }

        [[nodiscard]] const Header &getHeader() const {
            return _header;
        }

        void setHeader(uint8_t header) {
            _header.all = header;
        }

        [[nodiscard]] varInt getSize() const {
            return _size;
        }

        void setSize(varInt size) {
            _size = size;
        }

        virtual ~Message() = default;
    };

    struct ConnectMessage : public Message {
        std::string _protocolName{"MQIsdp"};
        uint8_t _protocolLevel{3};
        Flags _flags{0};
        std::string _clientId{};
        uint16_t _keepAlive{10};

        // connection data
        std::string _willTopic;
        std::string _willMessage;

        std::string _userName;
        std::string _password;
    public:
        ConnectMessage() : Message(MessageType::connect){ }

        ConnectMessage(std::string_view clientId) : Message(MessageType::connect), _clientId(clientId) { }

        [[nodiscard]] const std::string &getProtocolName() const {
            return _protocolName;
        }

        void setProtocolName(std::string_view protocolName) {
            _protocolName = protocolName;
        }

        [[nodiscard]] uint8_t getProtocolLevel() const {
            return _protocolLevel;
        }

        void setProtocolLevel(uint8_t protocolLevel) {
            _protocolLevel = protocolLevel;
        }

        [[nodiscard]] const std::string &getWillTopic() const {
            return _willTopic;
        }

        void setWillTopic(std::string_view willTopic) {
            _willTopic = willTopic;
        }

        void setWillTopic(std::string_view willTopic, std::string_view willMessage) {
            _willTopic = willTopic;
            _willMessage = willMessage;
        }

        [[nodiscard]] const std::string &getWillMessage() const {
            return _willMessage;
        }

        void setWillMessage(std::string_view willMessage) {
            _willMessage = willMessage;
        }

        [[nodiscard]] const std::string &getUserName() const {
            return _userName;
        }

        void setUserName(std::string_view userName) {
            _userName = userName;
        }

        [[nodiscard]] const std::string &getPassword() const {
            return _password;
        }

        void setPassword(std::string_view password) {
            _password = password;
        }

        [[nodiscard]] const Flags &getFlags() const {
            return _flags;
        }

        void setFlags(uint8_t flags) {
            _flags.all = flags;
        }

        [[nodiscard]] const std::string &getClientId() const {
            return _clientId;
        }

        void setClientId(std::string_view clientId) {
            _clientId = clientId;
        }

        [[nodiscard]] uint16_t getKeepAlive() const {
            return _keepAlive;
        }

        void setKeepAlive(uint16_t keepAlive) {
            _keepAlive = keepAlive;
        }
    };

    enum ConnRespCode {
        RESP_CODE_ACCEPTED,                         // DefaultConnection accepted
        RESP_CODE_UNACCEPTABLE_PROTOCOL_VERSION,    // DefaultConnection refused, unacceptable protocol version
        RESP_CODE_IDENTIFIER_REJECTED,              // DefaultConnection refused, identifier rejected
        RESP_CODE_SERVER_UNAVAILABLE,               // DefaultConnection refused, server unavailable
        RESP_CODE_BAD_USER_NAME_OR_PASSWORD,        // DefaultConnection refused, bad user name or password
        RESP_CODE_NOT_AUTHORIZED,                   // DefaultConnection refused, not authorized
    };

    struct ConnAckMessage : public Message {
        union {
            uint8_t all;    /**< all connack flags */
            struct {
                bool sessionPresent : 1;    /**< was a session found on the server? */
                unsigned int reserved : 7;    /**< message type nibble */
            } bits;
        } _flags{};     /**< connack flags byte */
        uint8_t _rc{}; /**< connack reason code */
    public:
        ConnAckMessage() : Message(MessageType::conn_ack){ }

        [[nodiscard]] uint8_t getReasonCode() const {
            return _rc;
        }
        [[nodiscard]] const char* getReasonCodeDescription() const {
            switch (_rc) {
                case RESP_CODE_ACCEPTED:
                    return "DefaultConnection accepted";
                case RESP_CODE_UNACCEPTABLE_PROTOCOL_VERSION:
                    return "DefaultConnection refused, unacceptable protocol version";
                case RESP_CODE_IDENTIFIER_REJECTED:
                    return "DefaultConnection refused, identifier rejected";
                case RESP_CODE_SERVER_UNAVAILABLE:
                    return "DefaultConnection refused, server unavailable";
                case RESP_CODE_BAD_USER_NAME_OR_PASSWORD:
                    return "DefaultConnection refused, bad user name or password";
                case RESP_CODE_NOT_AUTHORIZED:
                    return "DefaultConnection refused, not authorized";
            }

            return "Unknown response";
        }

        void setReasonCode(uint8_t rc) {
            _rc = rc;
        }

        [[nodiscard]] uint8_t getFlags() const {
            return _flags.all;
        }

        void setFlags(uint8_t flags) {
            _flags.all = flags;
        }
    };

    struct PingReqMessage : public Message {
    public:
        PingReqMessage() : Message(MessageType::ping_req){ }
    };

    struct PingRespMessage : public Message {
    public:
        PingRespMessage() : Message(MessageType::ping_resp){ }
    };
}