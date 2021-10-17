//
// Created by Ivan Kishchenko on 15.10.2021.
//

#include "ZeroMQCodec.h"

namespace network::zeromq {

    void ZeroMQCodec::handleRead(const Context &ctx, const ByteBufferRef<uint8_t> &buf) {
        _incBuf.append((char *) buf.data(), (std::streamsize) buf.size());
        if (_state == ZeroMQState::Greeting) {
            if (_incBuf.size() < 64) {
                return;
            }

            if (_incBuf.data()[0] != -1 || _incBuf.data()[9] != 0x7f) {
                fireShutdown();
                return;
            }

            ZeroMQGreeting greeting(false);
            std::istream inc(&_incBuf);
            inc >> greeting;
            _incBuf.consume(64);

            _decoder.onCommand([this, ctx](ZeroMQCommand &msg) {
                fireMessage(ctx, msg);
            });
            _decoder.onMessage([this, ctx](ZeroMQMessage &msg) {
                fireMessage(ctx, msg);
            });

            fireActive(ctx);
            _state = ZeroMQState::Stream;
        } else {
            while (!_decoder.read(_incBuf)) {}
        }
    }

    void ZeroMQCodec::handleActive(const Context &ctx) {
        ByteBufFix<64> buf;
        ZeroMQGreeting greeting(true);
        _encoder.write(buf, greeting);
        write(ctx, ByteBufferRef<uint8_t>{buf});
    }

    void ZeroMQCodec::handleWrite(const Context &ctx, const ZeroMQCommand &msg) {
        ByteBufFix<2048> buf;
        _encoder.write(buf, msg);

        write(ctx, ByteBufferRef<uint8_t>{buf});
    }

    void ZeroMQCodec::handleWrite(const Context &ctx, const ZeroMQMessage &msg) {
        ByteBufFix<2048> buf;
        _encoder.write(buf, msg);
        write(ctx, ByteBufferRef<uint8_t>{buf});
    }

}
