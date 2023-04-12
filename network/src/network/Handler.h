//
// Created by Ivan Kishchenko on 10.10.2021.
//

#pragma once

#include <memory>
#include <array>
#include "Logging.h"

namespace network {

    struct Context {
        std::string address;
        uint16_t port;
    };

    class Void {
    };

    enum UserMsgId {
        read_idle,
        write_idle,
    };

    class UserMessage {
        int _id;

    public:
        UserMessage(int id)
                : _id(id) {}

        int getId() const {
            return _id;
        }
    };

    class NetworkHandler {
    public:
        virtual void handleActive(const Context &ctx) {};

        virtual void handleInactive(const Context &ctx) {};

        virtual void handleError(const Context &ctx, std::error_code err) {};

        virtual void handleUserMessage(const Context &ctx, const UserMessage &err) {};
    };

    class StreamHandler {
    public:
        virtual void handleShutdown() {}

    };

    template<typename T, typename... Tn>
    class InboundHandler : public InboundHandler<T>, public InboundHandler<Tn...> {
    public:
        using InboundHandler<T>::handleRead;
        using InboundHandler<Tn...>::handleRead;
    };

    template<typename T>
    class InboundHandler<T> : public virtual NetworkHandler {
    public:
        virtual void handleRead(const Context &ctx, const T &msg) = 0;

        virtual ~InboundHandler() = default;
    };

    class Next {
    };

    template<typename T, typename ...Tn>
    class NextLink : Next {
        std::shared_ptr<InboundHandler<T, Tn...>> _next;
    public:

        template<typename Msg>
        void fireMessage(const Context &ctx, const Msg &msg) {
            if (_next) {
                _next->handleRead(ctx, msg);
            }
        }

        void linkNext(std::shared_ptr<InboundHandler<T, Tn...>> next) {
            _next = next;
        }

        auto getNext() {
            return _next;
        }

        virtual void fireActive(const Context &ctx) {
            if (_next) {
                _next->handleActive(ctx);
            }
        }

        virtual void fireInactive(const Context &ctx) {
            if (_next) {
                _next->handleInactive(ctx);
            }
        }

        virtual void fireError(const Context &ctx, std::error_code err) {
            if (_next) {
                _next->handleError(ctx, err);
            }
        }

        virtual void fireUserMessage(const Context &ctx, const UserMessage &msg) {
            if (_next) {
                _next->handleUserMessage(ctx, msg);
            }
        }
    };

    template<typename T, typename... Tn>
    class OutboundHandler : public OutboundHandler<T>, public OutboundHandler<Tn...> {
    public:
        using OutboundHandler<T>::handleWrite;
        using OutboundHandler<Tn...>::handleWrite;
    };

    template<typename T>
    class OutboundHandler<T> : public virtual StreamHandler {
    public:
        virtual void handleWrite(const Context &ctx, const T &msg) = 0;

        virtual ~OutboundHandler() = default;
    };

    class Prev {
    };

    template<typename T, typename ...Tn>
    class PrevLink : Prev {
        std::weak_ptr<OutboundHandler<T, Tn...>> _prev;
    public:

        template<typename Msg>
        void write(const Context &ctx, const Msg &msg) {
            if (auto prev = _prev.lock(); prev) {
                prev->handleWrite(ctx, msg);
            }
        }

        void linkPrev(std::shared_ptr<OutboundHandler<T, Tn...>> prev) {
            _prev = prev;
        }

        virtual void fireShutdown() {
            if (auto prev = _prev.lock(); prev) {
                prev->handleShutdown();
            }
        }
    };

    template<typename T, typename ...Tn>
    class InboundMessageHandler : public InboundHandler<T>, public NextLink<Tn...> {
    public:
        void handleActive(const Context &ctx) override {
            this->fireActive(ctx);
        }

        void handleInactive(const Context &ctx) override {
            this->fireInactive(ctx);
        }

        void handleError(const Context &ctx, std::error_code err) override {
            this->fireError(ctx, err);
        }

        void handleUserMessage(const Context &ctx, const UserMessage &msg) override {
            this->fireUserMessage(ctx, msg);
        }
    };

    template<typename T, typename ...Tn>
    class OutboundMessageHandler : public OutboundHandler<Tn...>, public PrevLink<T> {
    public:
        void handleShutdown() override {
            this->fireShutdown();
        }
    };

    template<typename T, typename ...Tn>
    class InboundOutboundMessageHandler
            : public InboundMessageHandler<T, Tn...>, public OutboundMessageHandler<T, Tn...> {
    };

    template<typename A, typename B>
    void link(std::shared_ptr<A> a, std::shared_ptr<B> b) {
        a->linkNext(b);

        if constexpr (std::is_base_of<Prev, B>::value) {
            b->linkPrev(a);
        }
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
    auto
    link(std::shared_ptr<A> a, std::shared_ptr<B> b, std::shared_ptr<C> c, std::shared_ptr<D> d, std::shared_ptr<E> e) {
        link(a, b, c, d);
        link(d, e);

        return a;
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F>
    auto
    link(std::shared_ptr<A> a, std::shared_ptr<B> b, std::shared_ptr<C> c, std::shared_ptr<D> d, std::shared_ptr<E> e,
         std::shared_ptr<F> f) {
        link(a, b, c, d, e);
        link(e, f);

        return a;
    }
}