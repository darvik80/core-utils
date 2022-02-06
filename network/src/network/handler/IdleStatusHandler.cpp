//
// Created by Ivan Kishchenko on 06.02.2022.
//

#include "IdleStateHandler.h"

namespace network::handler {

    void IdleStateHandler::resetReadTimer(const Context &ctx) {
        _readTimer.expires_from_now(_readTimeout);
        _readTimer.async_wait([ctx, this](boost::system::error_code err) {
            if (!err) {
                fireUserMessage(ctx, UserMessage(read_idle));
            }
        });
    }

    void IdleStateHandler::resetWriteTimer(const Context &ctx) {
        _writeTimer.expires_from_now(_writeTimeout);
        _writeTimer.async_wait([ctx, this](boost::system::error_code err) {
            if (!err) {
                fireUserMessage(ctx, UserMessage(write_idle));
            }
        });
    }

    void IdleStateHandler::handleActive(const Context &ctx) {
        InboundMessageHandler::handleActive(ctx);
        resetReadTimer(ctx);
        resetWriteTimer(ctx);
    }

    void IdleStateHandler::handleRead(const Context &ctx, const Buffer &msg) {
        resetReadTimer(ctx);
        fireMessage(ctx, msg);
    }

    void IdleStateHandler::handleWrite(const Context &ctx, const Buffer &msg) {
        resetWriteTimer(ctx);
        write(ctx, msg);
    }
}
