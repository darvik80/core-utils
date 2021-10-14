//
// Created by Ivan Kishchenko on 13.10.2021.
//

#pragma once

#include "network/Handler.h"
#include <string>

class NetworkLogger : public MessageHandler<ByteBuf, ByteBuf> {
private:
    std::string dump(const ByteBuf& buf);

public:
    void handleRead(const ByteBuf &event) override;

    void handleWrite(const ByteBuf &event) override;
};


