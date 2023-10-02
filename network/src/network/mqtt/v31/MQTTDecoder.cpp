//
// Created by Ivan Kishchenko on 27.12.2022.
//
#include "MQTTDecoder.h"
#include "network/mqtt/MQTTLogging.h"

void network::mqtt::v31::MQTTDecoder::handleReadConnect(network::Reader &inc) {
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

void network::mqtt::v31::MQTTDecoder::handleReadConnAck(network::Reader &inc) {
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

void network::mqtt::v31::MQTTDecoder::handleReadPingReq(network::Reader &inc) {
    PingReqMessage msg;
    /// 3.2.1 Fixed header
    inc >> msg._header.all << IOFlag::variable >> msg._size;

    _pingHandler(msg);
}

void network::mqtt::v31::MQTTDecoder::handleReadPingResp(network::Reader &inc) {
    PingRespMessage msg;
    /// 3.2.1 Fixed header
    inc >> msg._header.all << IOFlag::variable >> msg._size;

    _pongHandler(msg);
}

void network::mqtt::v31::MQTTDecoder::handleReadPublish(network::Reader &inc) {
    PublishMessage msg;
    uint16_t restSize;
    /// 3.2.1 Fixed header
    inc >> msg._header.all << IOFlag::variable >> restSize;

    msg._size = restSize;

    uint16_t size;
    inc << IOFlag::be >> size;
    inc << size >> msg._topic;
    restSize -= 2 + size;

    if (msg._header.bits.qos) {
        /// 3.3.2.2 Packet Identifier
        inc << IOFlag::be >> msg._packetIdentifier;
        restSize -= 2;
    }

    inc << restSize >> msg._message;

    if (_pubHandler) {
        _pubHandler(msg);
    }
}

void network::mqtt::v31::MQTTDecoder::handleReadPubAck(network::Reader &inc) {
    PubAckMessage msg;

    /// 3.4.1 Fixed header
    inc >> msg._header.all << IOFlag::variable >> msg._size;

    /// 3.4.2 Variable header
    inc << IOFlag::be >> msg._packetIdentifier;

    _pubAckHandler(msg);
}

void network::mqtt::v31::MQTTDecoder::handleReadPubComp(network::Reader &inc) {
    PubCompMessage msg;

    /// 3.7.1 Fixed header
    inc >> msg._header.all << IOFlag::variable >> msg._size;

    /// 3.7.2 Variable header
    inc << IOFlag::be >> msg._packetIdentifier;
}

void network::mqtt::v31::MQTTDecoder::handleReadSubscribe(network::Reader &inc) {
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

void network::mqtt::v31::MQTTDecoder::handleReadSubAck(network::Reader &inc) {
    SubAckMessage msg;

    /// 3.9.1 Fixed header
    inc >> msg._header.all << IOFlag::variable >> msg._size;

    /// 3.9.2 Variable header
    inc << IOFlag::be >> msg._packetIdentifier;

    /// 3.9.3 Payload
    inc >> msg._returnCode;

    _subAckHandler(msg);
}

void network::mqtt::v31::MQTTDecoder::handleReadUnSubscribe(network::Reader &inc) {
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

void network::mqtt::v31::MQTTDecoder::handleReadUnSubAck(network::Reader &inc) {
    UnSubAckMessage msg;

    /// 3.11.1 Fixed header
    inc >> msg._header.all << IOFlag::variable >> msg._size;

    /// 3.11.2 Variable header
    inc << IOFlag::be >> msg._packetIdentifier;

    _unSubAckHandler(msg);
}

std::error_code network::mqtt::v31::MQTTDecoder::read(network::Buffer &buf) {
    while (buf.size()) {
        mqtt::log::info("decode msg: {}", buf.size());
        Reader hdr(buf);

        Header header{};
        varInt size = 0;
        if (hdr >> header.all << IOFlag::variable >> size; !hdr) {
            mqtt::log::info("decode msg-hdr: {}", buf.size());
            return std::make_error_code(std::errc::message_size);
        }

        if (hdr.available() < size) {
            mqtt::log::info("decode msg-body: {}", buf.size());
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
            case MessageType::pub_comp:
                handleReadPubComp(inc);
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
                mqtt::log::info("unknown msg: {}", (int)header.bits.type);
                break;

        }

        mqtt::log::info("decode after msg: {}:{}", buf.size(), inc.consumed());

        buf.consume(inc.consumed());
    }

    return {};
}
