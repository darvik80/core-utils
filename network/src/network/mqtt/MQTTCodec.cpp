//
// Created by Ivan Kishchenko on 10.12.2021.
//

#include "MQTTCodec.h"

namespace network::mqtt {
    void MQTTCodec::handleReadConnect(const Context &ctx, Reader &inc) {
        ConnectMessage msg;
        inc >> msg._header.all >> msg._size >> msg._protocolName;

        /// 3.1.2.2 Protocol Level
        inc >> msg._protocolLevel;
        if (msg.getProtocolLevel() != 3) {
            fireShutdown();
            return;
        }
        /// 3.1.2.3 Connect Flags
        inc >>msg._flags.all;

        /// 3.1.2.10 Keep Alive
        inc >>msg._keepAlive;
        /// 3.1.3.1 DefaultConnection Identifier
        inc >>msg._clientId;

        if (msg.getFlags().bits.willFlag) {
            /// 3.1.3.2 Will Topic
            uint16_t size;
            inc >> size;
            inc.read(size, msg._willTopic);
            /// 3.1.3.3 Will Message
            inc >> size;
            inc.read(size, msg._willMessage);
        }

        if (msg.getFlags().bits.username) {
            /// 3.1.3.4 User Name
            uint16_t size;
            inc >> size;
            inc.read(size, msg._userName);
        }

        if (msg.getFlags().bits.password) {
            /// 3.1.3.5 Password
            uint16_t size;
            inc >> size;
            inc.read(size, msg._password);
        }

        fireMessage(ctx, msg);
    }

    void MQTTCodec::handleReadConnAck(const Context &ctx, Reader &inc) {
        ConnAckMessage msg;
        /// 3.2.1 Fixed header
        inc >> msg._header.all >> msg._size;

        /// 3.2.2 Variable header
        /// 3.2.2.1 Connect Acknowledge Flags
        /// 3.2.2.2 Session Present
        inc >> msg._flags.all;

        /// 3.2.2.3 Connect Return code
        inc >> msg._rc;

        fireMessage(ctx, msg);
    }

    void MQTTCodec::handleReadPingReq(const Context &ctx, Reader &inc) {
        PingReqMessage msg;
        /// 3.12.1 Fixed header
        inc >> msg._header.all >> msg._size;

        fireMessage(ctx, msg);
    }

    void MQTTCodec::handleReadPingResp(const Context &ctx, Reader &inc) {
        PingRespMessage msg;
        /// 3.13.1 Fixed header
        inc >> msg._header.all >> msg._size;

        fireMessage(ctx, msg);
    }

    void MQTTCodec::handleRead(const network::Context &ctx, const network::Buffer &msg) {
        _incBuf.append(msg.data(), msg.size());

        if (_incBuf.size()) {
            Reader hdr(_incBuf);

            Header header{};
            varInt size;
            if (hdr >> header.all >> size; !hdr) {
                return;
            }

            if (hdr.available() < size) {
                return;
            }

            Reader inc(_incBuf);
            switch ((MessageType) header.bits.type) {
                case MessageType::connect:
                    handleReadConnect(ctx, inc);
                    break;
                case MessageType::conn_ack:
                    handleReadConnect(ctx, inc);
                    break;
                case MessageType::ping_req:
                    handleReadConnect(ctx, inc);
                    break;
                case MessageType::ping_resp:
                    handleReadConnect(ctx, inc);
                    break;
                default:
                    break;

            }

            _incBuf.consume(inc.consumed());
        }
    }

    void MQTTCodec::handleWrite(const network::Context &ctx, const network::mqtt::ConnectMessage &msg) {
        ArrayBuffer<1024> buf;
        Writer writer(buf);

        size_t res = 0;

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

        ArrayBuffer<1024> data;
        Writer out(data);
        out << msg.getHeader().all << IOFlag::variable << writer.available() << data;

        write(ctx, data);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const ConnAckMessage &msg) {
        ArrayBuffer<32> buf;
        Writer out(buf);

        out << msg.getHeader().all << IOFlag::variable << 2;

        /// 3.2.2 Variable header
        /// 3.2.2.1 Connect Acknowledge Flags
        out << msg.getFlags();

        /// 3.2.2.2 Session Present
        /// 3.2.2.3 Connect Return code
        out << msg.getReasonCode();

        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const PingReqMessage &msg) {
        ArrayBuffer<4> buf;
        Writer out(buf);

        out << msg.getHeader().all << IOFlag::variable << 0;

        write(ctx, buf);
    }

    void MQTTCodec::handleWrite(const Context &ctx, const PingRespMessage &msg) {
        ArrayBuffer<4> buf;
        Writer out(buf);

        out << msg.getHeader().all << IOFlag::variable << 0;

        write(ctx, buf);
    }
}
