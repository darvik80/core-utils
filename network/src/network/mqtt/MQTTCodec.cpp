//
// Created by Ivan Kishchenko on 10.12.2021.
//

#include "MQTTCodec.h"

namespace network::mqtt {
    void MQTTCodec::handleReadConnect(const Context &ctx, std::istream &inc) {
        MQTTReader reader;

        ConnectMessage msg;
        msg.setHeader(reader.readUint8(inc));
        msg.setSize(reader.readVariableInt(inc));

        msg.setProtocolName(reader.readString(inc));
        /// 3.1.2.2 Protocol Level
        msg.setProtocolLevel(reader.readUint8(inc));
        if (msg.getProtocolLevel() != 3) {
            fireShutdown();
            return;
        }
        /// 3.1.2.3 Connect Flags
        msg.setFlags(reader.readUint8(inc));

        /// 3.1.2.10 Keep Alive
        msg.setKeepAlive(reader.readUint16(inc));
        /// 3.1.3.1 DefaultConnection Identifier
        msg.setClientId(reader.readString(inc));

        if (msg.getFlags().bits.willFlag) {
            /// 3.1.3.2 Will Topic
            msg.setWillTopic(reader.readString(inc));
            /// 3.1.3.3 Will Message
            msg.setWillMessage(reader.readString(inc));
        }

        if (msg.getFlags().bits.username) {
            /// 3.1.3.4 User Name
            msg.setUserName(reader.readString(inc));
        }

        if (msg.getFlags().bits.password) {
            /// 3.1.3.5 Password
            msg.setPassword(reader.readString(inc));
        }

        fireMessage(ctx, msg);
    }

    void MQTTCodec::handleReadConnAck(const Context &ctx, std::istream &inc) {
        MQTTReader reader;

        ConnAckMessage msg;
        /// 3.2.1 Fixed header
        msg.setHeader(reader.readUint8(inc));
        msg.setSize(reader.readVariableInt(inc));

        /// 3.2.2 Variable header
        /// 3.2.2.1 Connect Acknowledge Flags
        /// 3.2.2.2 Session Present
        msg.setFlags(reader.readUint8(inc));

        /// 3.2.2.3 Connect Return code
        msg.setReasonCode(reader.readUint8(inc));

        fireMessage(ctx, msg);
    }

    void MQTTCodec::handleReadPingReq(const Context &ctx, std::istream &inc) {
        MQTTReader reader;

        PingReqMessage msg;
        /// 3.12.1 Fixed header
        msg.setHeader(reader.readUint8(inc));
        msg.setSize(reader.readVariableInt(inc));

        fireMessage(ctx, msg);
    }

    void MQTTCodec::handleReadPingResp(const Context &ctx, std::istream &inc) {
        MQTTReader reader;

        PingRespMessage msg;
        /// 3.13.1 Fixed header
        msg.setHeader(reader.readUint8(inc));
        msg.setSize(reader.readVariableInt(inc));

        fireMessage(ctx, msg);
    }

    void MQTTCodec::handleRead(const network::Context &ctx, const network::ByteBufferRef<uint8_t> &msg) {
        _incBuf.append((char *) msg.data(), (std::streamsize) msg.size());

        if (_incBuf.size()) {
            std::istream stream(&_incBuf);
            MQTTReader reader;

            Header header{};
            header.all = reader.readUint8(stream);

            size_t encSize = 0;
            int multiplier = 1;
            int result = 0;

            auto available = _incBuf.size() - 1;
            uint8_t encoded = 0;
            do {
                if (available--) {
                    return;
                }
                encSize++;
                encoded = reader.readUint8(stream);
                result += (encoded & 0x7F) * multiplier;
                if (multiplier > 0x80 * 0x80 * 0x80) {
                    fireShutdown();
                    return;
                }
                multiplier *= 0x80;
            } while ((encoded & 0x80) != 0);

            if (available < result) {
                return;
            }

            switch ((MessageType) header.bits.type) {
                case MessageType::connect:
                    handleReadConnect(ctx, stream);
                    break;
                case MessageType::conn_ack:
                    handleReadConnect(ctx, stream);
                    break;
                case MessageType::ping_req:
                    handleReadConnect(ctx, stream);
                    break;
                case MessageType::ping_resp:
                    handleReadConnect(ctx, stream);
                    break;
                default:
                    break;

            }
            _incBuf.consume(1 + encSize + available);
        }
    }

    void MQTTCodec::handleWrite(const network::Context &ctx, const network::mqtt::ConnectMessage &msg) {
        ByteBufFix<1024> buf;
        std::ostream stream(&buf);

        MQTTWriter writer;
        size_t res = 0;

        /// 3.1.2.1 Protocol name
        res += writer.writeString(msg.getProtocolName(), stream);
        /// 3.1.2.2 Protocol version
        res += writer.writeUint8(msg.getProtocolLevel(), stream);
        /// 3.1.2.2 Protocol flags
        res += writer.writeUint8(msg.getFlags().all, stream);
        /// 3.1.2.10 Keep alive
        res += writer.writeUint16(msg.getKeepAlive(), stream);

        /// 3.1.3 Payload
        /// 3.1.3.1 DefaultConnection Id
        res += writer.writeString(msg.getClientId(), stream);

        if (msg.getFlags().bits.willFlag) {
            /// 3.1.3.2 Will Topic
            res += writer.writeString(msg.getWillTopic(), stream);
            /// 3.1.3.3 Will Message
            res += writer.writeString(msg.getWillMessage(), stream);
        }

        if (msg.getFlags().bits.username) {
            /// 3.1.3.4 User Name
            res += writer.writeString(msg.getUserName(), stream);
        }

        if (msg.getFlags().bits.password) {
            /// 3.1.3.5 Password
            res += writer.writeString(msg.getPassword(), stream);
        }

        ByteBufFix<1024> data;
        std::ostream out(&data);

        writer.writeUint8(msg.getHeader().all, out);
        writer.writeVariableInt(res, out);
        writer.writeData(data, out);

        write(ctx, ByteBufferRef<uint8_t>{buf});
    }

    void MQTTCodec::handleWrite(const Context &ctx, const ConnAckMessage &msg) {
        ByteBufFix<32> buf;
        std::ostream out(&buf);

        MQTTWriter writer;
        size_t res = 0;

        writer.writeUint8(msg.getHeader().all, out);
        writer.writeVariableInt(2, out);

        /// 3.2.2 Variable header
        /// 3.2.2.1 Connect Acknowledge Flags
        res += writer.writeUint8(msg.getFlags(), out);
        /// 3.2.2.2 Session Present
        /// 3.2.2.3 Connect Return code
        res += writer.writeUint8(msg.getReasonCode(), out);

        write(ctx, ByteBufferRef<uint8_t>{buf});
    }

    void MQTTCodec::handleWrite(const Context &ctx, const PingReqMessage &msg) {
        ByteBufFix<4> buf;
        std::ostream out(&buf);

        MQTTWriter writer;
        writer.writeUint8(msg.getHeader().all, out);
        writer.writeVariableInt(0, out);

        write(ctx, ByteBufferRef<uint8_t>{buf});
    }

    void MQTTCodec::handleWrite(const Context &ctx, const PingRespMessage &msg) {
        ByteBufFix<4> buf;
        std::ostream out(&buf);

        MQTTWriter writer;
        writer.writeUint8(msg.getHeader().all, out);
        writer.writeVariableInt(0, out);

        write(ctx, ByteBufferRef<uint8_t>{buf});
    }
}
