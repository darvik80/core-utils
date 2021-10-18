//
// Created by Ivan Kishchenko on 15.10.2021.
//

#include "ZeroMQCodec.h"
#include "v31/ZeroMQDecoder.h"
#include "v31/ZeroMQEncoder.h"

#include "v30/ZeroMQDecoder.h"
#include "v30/ZeroMQEncoder.h"

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

            if (greeting.version.major == 0x03) {
                if (greeting.version.minor == 0x00) {
                    _decoder = std::make_shared<v30::ZeroMQDecoder>();
                    _encoder = std::make_shared<v30::ZeroMQEncoder>();
                } else if (greeting.version.minor == 0x01) {
                    _decoder = std::make_shared<v31::ZeroMQDecoder>();
                    _encoder = std::make_shared<v31::ZeroMQEncoder>();
                }
            } else {
                fireShutdown();
                return;
            }
            _decoder->onCommand([this, ctx](ZeroMQCommand &msg) {
                fireMessage(ctx, msg);
            });
            _decoder->onMessage([this, ctx](ZeroMQMessage &msg) {
                fireMessage(ctx, msg);
            });

            fireActive(ctx);
            _state = ZeroMQState::Stream;
        } else {
            std::error_code err{};
            while (!err) {
                err = _decoder->read(_incBuf);
            }
            if (err == std::errc::invalid_argument) {
                fireShutdown();
            }
        }
    }

    void ZeroMQCodec::handleActive(const Context &ctx) {
        ByteBufFix<64> buf;
        ZeroMQGreeting greeting(true);
        std::ostream out(&buf);
        out << greeting;

        write(ctx, ByteBufferRef<uint8_t>{buf});
    }

    void ZeroMQCodec::handleWrite(const Context &ctx, const ZeroMQCommand &msg) {
        ByteBufFix<2048> buf;
        _encoder->write(buf, msg);

        write(ctx, ByteBufferRef<uint8_t>{buf});
    }

    void ZeroMQCodec::handleWrite(const Context &ctx, const ZeroMQMessage &msg) {
        ByteBufFix<2048> buf;
        _encoder->write(buf, msg);

        write(ctx, ByteBufferRef<uint8_t>{buf});
    }

}
