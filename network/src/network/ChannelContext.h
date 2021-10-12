//
// Created by Ivan Kishchenko on 10.10.2021.
//

#pragma once

#include <memory>

class ChannelPipeline;
class Channel;
class Handler;

class ChannelContext {
    std::shared_ptr<ChannelPipeline> pipeline;
    std::shared_ptr<Handler> handler;

    std::weak_ptr<ChannelContext> prev;
    std::shared_ptr<ChannelContext> next;

public:
    [[nodiscard]] const std::shared_ptr<ChannelPipeline> &getPipeline() const;

    [[nodiscard]] const std::shared_ptr<Handler> &getHandler() const;

    [[nodiscard]] const std::weak_ptr<ChannelContext> &getPrev() const;

    [[nodiscard]] const std::shared_ptr<ChannelContext> &getNext() const;

    void handleActive();

    void handleRead();
    void handleWrite();

    void handleInactive();
};
