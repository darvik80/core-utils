//
// Created by Ivan Kishchenko on 10.10.2021.
//

#include "ChannelContext.h"
#include "Handler.h"

const std::shared_ptr<ChannelPipeline> &ChannelContext::getPipeline() const {
    return pipeline;
}

const std::shared_ptr<Handler> &ChannelContext::getHandler() const {
    return handler;
}

const std::weak_ptr<ChannelContext> &ChannelContext::getPrev() const {
    return prev;
}

const std::shared_ptr<ChannelContext> &ChannelContext::getNext() const {
    return next;
}

void ChannelContext::handleActive() {
    auto nextContext = this;
    while (true) {
        if (!(nextContext = nextContext->getNext().get())) {
            break;
        }

        auto nextHandler = dynamic_cast<ActiveHandler *>(nextContext->getHandler().get());
        if (nextHandler != nullptr) {
            nextHandler->handleActive(*nextContext);
        }
    }
}

void ChannelContext::handleRead() {
    auto nextContext = this;
    while (true) {
        if (nextContext = nextContext->getNext().get(); !nextContext) {
            break;
        }

        auto nextHandler = dynamic_cast<InboundHandler *>(nextContext->getHandler().get());
        if (nextHandler != nullptr) {
            nextHandler->handleRead(*nextContext);
        }
    }
}

void ChannelContext::handleWrite() {
    auto prevContext = this;
    while (true) {
        if (prevContext = prevContext->getPrev().lock().get(); !prevContext) {
            break;
        }

        auto prevHandler = dynamic_cast<OutboundHandler *>(prevContext->getHandler().get());
        if (prevHandler != nullptr) {
            prevHandler->handleWrite(*prevContext);
        }
    }
}


void ChannelContext::handleInactive() {
    auto nextContext = this;
    while (true) {
        if (nextContext = nextContext->getNext().get(); !nextContext) {
            break;
        }

        auto nextHandler = dynamic_cast<InactiveHandler *>(nextContext->getHandler().get());
        if (nextHandler != nullptr) {
            nextHandler->handleInactive(*nextContext);
        }
    }
}
