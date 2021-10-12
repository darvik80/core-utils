//
// Created by Ivan Kishchenko on 10.10.2021.
//

#pragma once

#include <list>
#include <utility>
#include "Channel.h"

class Handler;

struct HandlerContext {
    HandlerContext* prev;
    HandlerContext* next;
    Handler* handler;
};

class Handler {
public:
    virtual void onActive(HandlerContext& ctx) = 0;

    virtual void onReceive(HandlerContext& ctx, const void* data, size_t size) = 0;
    virtual void onSend(HandlerContext& ctx, size_t size, std::error_code err) = 0;

    virtual void onError(HandlerContext& ctx, std::error_code err) = 0;
    virtual void onInactive(HandlerContext& ctx) = 0;
};

class HeadHandler : public Handler {
public:
    void onActive(HandlerContext &ctx) override {

    }

    void onReceive(HandlerContext &ctx, const void *data, size_t sizer) override {

    }

    void onSend(HandlerContext &ctx, size_t size, std::error_code err) override {

    }

    void onError(HandlerContext &ctx, std::error_code err) override {

    }

    void onInactive(HandlerContext &ctx) override {

    }
};

template<typename T>
class InboundHandler : public Handler {
public:
    void onActive(HandlerContext &ctx) override {

    }

    void onReceive(HandlerContext &ctx, const void *data, size_t size) override {

    }

    void onReceive(HandlerContext &ctx, T& msg) override {

    }

    void onSend(HandlerContext &ctx, size_t size, std::error_code err) override {

    }

    void onError(HandlerContext &ctx, std::error_code err) override {

    }

    void onInactive(HandlerContext &ctx) override {

    }
};

template<typename T>
struct InboundHandlerContext : HandlerContext {
    InboundHandler<T> handler;
    void handleRead(T& msg) {
        handler.onReceive(*this, msg);
    }
};

class HandlerPipeline {
    HandlerContext* _head;
    HandlerContext* _cur;
private:
    template<typename T, typename ... Ts>
    void add(InboundHandler<T>* handler, Ts &&... handlers) {
        if (_head == nullptr) {
            _head = new HandlerContext{nullptr, nullptr, new HeadHandler()};
            _cur = _head;
        }

        _cur->next = new HandlerContext{_cur, nullptr, handler};
        _cur = _cur->next;


        add(std::forward(handlers)...);
    }
public:
    template<typename ... T>
    void build(T &&... handlers) {
        add(std::forward(handlers)...);
    }
};

class ChannelPipeline {
    std::list<Channel::Ptr> _channels;
public:
    typedef std::shared_ptr<ChannelPipeline> Ptr;
    template<typename ... T>
    void addLast(T &&... channels) {
        for (auto channel : { channels... }) {
            _channels.emplace_back(channel);
        }
    }

    template<typename ... T>
    void addFirst(T &&... channels) {
        for (auto channel : { channels... }) {
            _channels.push_front(channel);
        }
    }
};


