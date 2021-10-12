//
// Created by Ivan Kishchenko on 10.10.2021.
//

#pragma once

#include <memory>
#include "NettworkLogger.h"

class ActiveHandler {
public:
    virtual void handleActive() {}
};

class InactiveHandler {
public:
    virtual void handleInactive() {}
};

class NetworkHandler : public ActiveHandler, public InactiveHandler {};

template<typename T>
class InboundHandler {
public:
    virtual void handleRead(const T &event) = 0;

    virtual ~InboundHandler() = default;
};

template<typename T>
class NetworkTrigger {
    std::shared_ptr<InboundHandler<T>> _next{};
public:
    virtual void trigger(const T &event) {
        if (_next) {
            _next->handleRead(event);
        }
    }

    void setNext(std::shared_ptr<InboundHandler<T>> next) {
        _next = next;
    }
};


template<typename T, typename U>
class InboundMessageHandler : public InboundHandler<T>, public NetworkTrigger<U> {
};

template<typename T>
class OutboundHandler {
public:
    virtual void handleWrite(const T &event) {}

    virtual ~OutboundHandler() = default;
};

template<typename T>
class NetworkWriter {
    std::weak_ptr<OutboundHandler<T>> _prev{};
public:
    void write(const T &event) {
        if (auto prev = _prev.lock()) {
            prev->handleWrite(event);
        }
    }

    void setPrev(std::weak_ptr<OutboundHandler<T>> next) {
        _prev = next;
    }
};

template<typename T, typename U>
class OutboundMessageHandler : public OutboundHandler<T>, public NetworkWriter<U> {
};

template<typename T, typename U>
class MessageHandler : public NetworkHandler, public InboundMessageHandler<T, U>, public OutboundMessageHandler<U, T> {
};
