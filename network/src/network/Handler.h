//
// Created by Ivan Kishchenko on 10.10.2021.
//

#pragma once

#include <memory>
#include <array>
#include "NetworkLogging.h"

namespace network {
    class Void {
    };

    class ActiveHandler {
    public:
        virtual void handleActive() {}
    };

    class InactiveHandler {
    public:
        virtual void handleInactive() {}
    };

    class ErrorHandler {
    public:
        virtual void handleError(std::error_code err) {}
    };

    class NetworkHandler : public ActiveHandler, public InactiveHandler, public ErrorHandler {
    public:
        virtual void shutdown() {};
    };

    template<typename T>
    class InboundHandler {
    public:
        virtual void handleRead(T &event) = 0;

        virtual ~InboundHandler() = default;
    };

    template<typename T>
    class NetworkTrigger {
        std::shared_ptr<InboundHandler<T>> _next{};
    public:
        virtual void trigger(T &event) {
            if (_next) {
                _next->handleRead(event);
            }
        }

        void setNext(std::shared_ptr<InboundHandler<T>> next) {
            _next = next;
        }

        std::shared_ptr<InboundHandler<T>> getNext() {
            return _next;
        }
    };


    template<typename T, typename U>
    class InboundMessageHandler : public InboundHandler<T>, public NetworkTrigger<U> {
    };

    template<typename T>
    class OutboundHandler {
    public:
        virtual void handleWrite(T &event) {}

        virtual ~OutboundHandler() = default;
    };

    template<typename T>
    class NetworkWriter {
        std::weak_ptr<OutboundHandler<T>> _prev{};
    public:
        virtual void write(T &event) {
            if (auto prev = _prev.lock()) {
                prev->handleWrite(event);
            }
        }

        void setPrev(std::weak_ptr<OutboundHandler<T>> next) {
            _prev = next;
        }

        std::shared_ptr<OutboundHandler<T>> getPrev() {
            return _prev.lock();
        }

    };

    template<typename T, typename U>
    class OutboundMessageHandler : public OutboundHandler<T>, public NetworkWriter<U> {
    };

    template<typename T, typename U = Void>
    class MessageHandler : public NetworkHandler, public InboundMessageHandler<T, U>, public OutboundMessageHandler<U, T> {
    public:
        void handleActive() override {
            auto handler = std::dynamic_pointer_cast<NetworkHandler>(this->getNext());
            if (handler) {
                handler->handleActive();
            }
        }

        void handleInactive() override {
            auto handler = std::dynamic_pointer_cast<NetworkHandler>(this->getNext());
            if (handler) {
                handler->handleInactive();
            }
        }

        void handleError(std::error_code err) override {
            auto handler = std::dynamic_pointer_cast<NetworkHandler>(this->getNext());
            if (handler) {
                handler->handleError(err);
            }
        }

        void shutdown() override {
            auto handler = std::dynamic_pointer_cast<NetworkHandler>(this->getPrev());
            if (handler) {
                handler->shutdown();
            }
        }
    };

    template<typename A, typename B>
    auto link(std::shared_ptr<A> a, std::shared_ptr<B> b) {
        a->setNext(b);
        b->setPrev(a);

        return a;
    }

    template<typename A, typename B, typename C>
    auto link(std::shared_ptr<A> a, std::shared_ptr<B> b, std::shared_ptr<C> c) {
        link(a, b);
        link(b, c);

        return a;
    }

    template<typename A, typename B, typename C, typename D>
    auto link(std::shared_ptr<A> a, std::shared_ptr<B> b, std::shared_ptr<C> c, std::shared_ptr<D> d) {
        link(a, b, c);
        link(c, d);

        return a;
    }

    template<typename A, typename B, typename C, typename D, typename E>
    auto link(std::shared_ptr<A> a, std::shared_ptr<B> b, std::shared_ptr<C> c, std::shared_ptr<D> d, std::shared_ptr<E> e) {
        link(a, b, c, d);
        link(d, e);

        return a;
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F>
    auto link(std::shared_ptr<A> a, std::shared_ptr<B> b, std::shared_ptr<C> c, std::shared_ptr<D> d, std::shared_ptr<E> e, std::shared_ptr<F> f) {
        link(a, b, c, d, e);
        link(e, f);

        return a;
    }
}