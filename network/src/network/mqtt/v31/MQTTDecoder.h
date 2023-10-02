//
// Created by Ivan Kishchenko on 05.02.2022.
//

#pragma once

#include "network/mqtt/MQTTDecoder.h"

namespace network::mqtt::v31 {

    class MQTTDecoder : public network::mqtt::MQTTDecoder {
    private:
        void handleReadConnect(Reader &inc);

        void handleReadConnAck(Reader &inc);

        void handleReadPingReq(Reader &inc);

        void handleReadPingResp(Reader &inc);

        void handleReadPublish(Reader &inc);

        void handleReadPubAck(Reader &inc);

        void handleReadPubComp(Reader &inc);

        void handleReadSubscribe(Reader &inc);

        void handleReadSubAck(Reader &inc);

        void handleReadUnSubscribe(Reader &inc);

        void handleReadUnSubAck(Reader &inc);

    public:
        std::error_code read(Buffer &buf) override;
    };
}

