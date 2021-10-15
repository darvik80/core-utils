//
// Created by Ivan Kishchenko on 15.10.2021.
//

#include "ZeroMQCodec.h"

namespace network::zeromq {

    void ZeroMQCodec::handleRead(ByteBuffer &buf) {
        if (_state == ZeroMQState::Greeting) {
            if (buf.size() < 64) {
                return;
            }

            if (buf.data()[0] != -1 || buf.data()[9] != 0x7f) {
                shutdown();
                return;
            }

            ZeroMQGreeting greeting(false);
            std::istream inc(&buf);
            inc >> greeting;
            _state = ZeroMQState::Stream;
            _decoder.onCommand([this](ZeroMQCommand &msg) {
                ZeroMQMsg message = msg;
                trigger(message);
            });
            buf.consume(64);
            _decoder.onMessage([this](ZeroMQMessage &msg) {
                ZeroMQMsg message = msg;
                trigger(message);
            });

            MessageHandler::handleActive();
        } else {
            _decoder.read(buf);
        }
    }

    void ZeroMQCodec::handleActive() {
        ByteBufFix<64> buf;
        std::ostream out(&buf);
        ZeroMQGreeting greeting(true);
        out << greeting;

        write(buf);
    }

    void ZeroMQCodec::handleWrite(ZeroMQMsg &event) {
        OutboundHandler::handleWrite(event);

        std::visit([this](auto &&arg) {
            ByteBufFix<2048> buf;
            _encoder.write(buf, arg);
            write(buf);
        }, event);
    }
}
