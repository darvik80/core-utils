//
// Created by Ivan Kishchenko on 10.10.2021.
//

#pragma once

#include <system_error>
#include <memory>

#include "ChannelContext.h"

class Channel {
public:
    typedef std::shared_ptr<Channel> Ptr;
public:
    virtual void onActive(ChannelContext& ctx) = 0;

    virtual void onReceive(ChannelContext& ctx, const void* data, size_t size, std::error_code err) = 0;
    virtual void onSend(ChannelContext& ctx, size_t size, std::error_code err) = 0;

    virtual void onError(ChannelContext& ctx, std::error_code err) = 0;
    virtual void onInactive(ChannelContext& ctx) = 0;
};

class SimpleChannel : public Channel {
public:
    void onActive(ChannelContext &ctx) override {

    }

    void onReceive(ChannelContext &ctx, const void *data, size_t size, std::error_code err) override {

    }

    void onSend(ChannelContext &ctx, size_t size, std::error_code err) override {

    }

    void onError(ChannelContext &ctx, std::error_code err) override {

    }

    void onInactive(ChannelContext &ctx) override {

    }
};
