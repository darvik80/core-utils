//
// Created by Ivan Kishchenko on 05.02.2022.
//

#pragma once

#include "network/mqtt/MQTTDecoder.h"

namespace network::mqtt::v31 {

    class MQTTDecoder : public network::mqtt::MQTTDecoder {
    private:
        void handleReadConnect(Reader &inc) {
            ConnectMessage msg;
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            uint16_t size;
            inc << IOFlag::be >> size;
            inc << size >> msg._protocolName;

            /// 3.1.2.2 Protocol Level
            inc << IOFlag::be >> msg._protocolLevel;
            if (msg.getProtocolLevel() != 3) {
                return;
            }
            /// 3.1.2.3 Connect Flags
            inc >> msg._flags.all;

            /// 3.1.2.10 Keep Alive
            inc >> msg._keepAlive;
            /// 3.1.3.1 DefaultConnection Identifier
            inc << IOFlag::be >> size;
            inc << size >> msg._clientId;

            if (msg.getFlags().bits.willFlag) {
                /// 3.1.3.2 Will Topic
                inc >> size;
                inc.read(size, msg._willTopic);
                /// 3.1.3.3 Will Message
                inc >> size;
                inc.read(size, msg._willMessage);
            }

            if (msg.getFlags().bits.username) {
                /// 3.1.3.4 User Name
                inc << IOFlag::be >> size;
                inc.read(size, msg._userName);
            }

            if (msg.getFlags().bits.password) {
                /// 3.1.3.5 Password
                inc << IOFlag::be >> size;
                inc.read(size, msg._password);
            }

            _connHandler(msg);
        }

        void handleReadConnAck(Reader &inc) {
            ConnAckMessage msg;
            /// 3.2.1 Fixed header
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            /// 3.2.2 Variable header
            /// 3.2.2.1 Connect Acknowledge Flags
            /// 3.2.2.2 Session Present
            inc >> msg._flags.all;

            /// 3.2.2.3 Connect Return code
            inc >> msg._rc;

            _connAckHandler(msg);
        }

        void handleReadPingReq(Reader &inc) {
            PingReqMessage msg;
            /// 3.2.1 Fixed header
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            _pingHandler(msg);
        }

        void handleReadPingResp(Reader &inc) {
            PingRespMessage msg;
            /// 3.2.1 Fixed header
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            _pongHandler(msg);
        }

        void handleReadPublish(Reader &inc) {
            PublishMessage msg;
            /// 3.2.1 Fixed header
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            uint16_t size;
            inc << IOFlag::be >> size;
            inc << size >> msg._topic;

            if (msg._header.bits.qos) {
                /// 3.3.2.2 Packet Identifier
                inc << IOFlag::be >> msg._packetIdentifier;
            }

            inc << IOFlag::be >> size;
            inc << size >> msg._message;

            if (_pubHandler) {
                _pubHandler(msg);
            }
        }

        void handleReadPubAck(Reader &inc) {
            PubAckMessage msg;

            /// 3.4.1 Fixed header
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            /// 3.4.2 Variable header
            inc << IOFlag::be >> msg._packetIdentifier;

            _pubAckHandler(msg);
        }

        void handleReadSubscribe(Reader &inc) {
            SubscribeMessage msg;

            /// 3.8.1 Fixed header
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            /// 3.8.2.1 Variable header non normative example
            inc << IOFlag::be >> msg._packetIdentifier;

            /// 3.8.3 Payload
            size_t msgSize = msg.getSize() - sizeof(uint16_t);
            while (msgSize > 0) {
                uint16_t size = 0;
                std::string topicFilter;
                uint8_t qos;

                inc << IOFlag::be >> size;
                inc << size >> topicFilter >> qos;
                msg.addTopic(topicFilter, qos);

                msgSize -= (sizeof(uint16_t) + topicFilter.size() + sizeof(uint8_t));
            }

            _subHandler(msg);
        }

        void handleReadSubAck(Reader &inc) {
            SubAckMessage msg;

            /// 3.9.1 Fixed header
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            /// 3.9.2 Variable header
            inc << IOFlag::be >> msg._packetIdentifier;

            /// 3.9.3 Payload
            inc >> msg._returnCode;

            _subAckHandler(msg);
        }

        void handleReadUnSubscribe(Reader &inc) {
            UnSubscribeMessage msg;

            /// 3.8.1 Fixed header
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            /// 3.8.2.1 Variable header non normative example
            inc << IOFlag::be >> msg._packetIdentifier;

            /// 3.8.3 Payload
            size_t msgSize = msg.getSize() - sizeof(uint16_t);
            while (msgSize > 0) {
                uint16_t size = 0;
                std::string topicFilter;

                inc << IOFlag::be >> size;
                inc << size >> topicFilter;
                msg.addTopicFilter(topicFilter);

                msgSize -= (sizeof(uint16_t) + topicFilter.size());
            }

            _unSubHandler(msg);
        }

        void handleReadUnSubAck(Reader &inc) {
            UnSubAckMessage msg;

            /// 3.11.1 Fixed header
            inc >> msg._header.all << IOFlag::variable >> msg._size;

            /// 3.11.2 Variable header
            inc << IOFlag::be >> msg._packetIdentifier;

            _unSubAckHandler(msg);
        }

    public:
        std::error_code read(Buffer &buf) override {
            Reader hdr(buf);

            Header header{};
            varInt size = 0;
            if (hdr >> header.all << IOFlag::variable >> size; !hdr) {
                return std::make_error_code(std::errc::message_size);
            }

            if (hdr.available() < size) {
                return std::make_error_code(std::errc::message_size);
            }

            Reader inc(buf);
            switch ((MessageType) header.bits.type) {
                case MessageType::connect:
                    handleReadConnect(inc);
                    break;
                case MessageType::conn_ack:
                    handleReadConnAck(inc);
                    break;
                case MessageType::ping_req:
                    handleReadPingReq(inc);
                    break;
                case MessageType::ping_resp:
                    handleReadPingResp(inc);
                    break;
                case MessageType::publish:
                    handleReadPublish(inc);
                    break;
                case MessageType::pub_ack:
                    handleReadPubAck(inc);
                    break;
                case MessageType::subscribe:
                    handleReadSubscribe(inc);
                    break;
                case MessageType::sub_ack:
                    handleReadSubAck(inc);
                    break;
                case MessageType::unsubscribe:
                    handleReadUnSubscribe(inc);
                    break;
                case MessageType::unsub_ack:
                    handleReadUnSubAck(inc);
                    break;
                default:
                    break;

            }

            buf.consume(inc.consumed());

            return {};
        }
    };
}

