//
// Created by Ivan Kishchenko on 05.02.2022.
//

#include "MQTTDecoderTest.h"

#include "MQTTEncoderTest.h"
#include "MQTTTestData.h"
#include "network/mqtt/v31/MQTTDecoder.h"

namespace network::mqtt::v31 {

    BOOST_FIXTURE_TEST_SUITE(MQTTDecoderV31Test, MQTTDecoderV31Fixture)

        BOOST_AUTO_TEST_CASE(testConnectEncode) {
            ArrayBuffer<64> buf;
            buf.append(rawConnect, sizeof(rawConnect));

            MQTTDecoder decoder;
            decoder.onConnect([](const ConnectMessage &msg) {
                BOOST_REQUIRE_EQUAL("paho/DDE4DDAF4108D3E363", msg.getClientId().c_str());
            });
            BOOST_REQUIRE_EQUAL(0, decoder.read(buf).value());
        }

        BOOST_AUTO_TEST_CASE(testConnAckEncode) {
            ArrayBuffer<64> buf;
            buf.append(rawConnAck, sizeof(rawConnAck));

            MQTTDecoder decoder;
            decoder.onConnAck([](const ConnAckMessage &msg) {
                BOOST_REQUIRE_EQUAL(0, msg.getReasonCode());
            });
            BOOST_REQUIRE_EQUAL(0, decoder.read(buf).value());
        }

        BOOST_AUTO_TEST_CASE(testPingReqEncode) {
            ArrayBuffer<64> buf;
            buf.append(rawPingReq, sizeof(rawPingReq));

            MQTTDecoder decoder;
            decoder.onPing([](const PingReqMessage &msg) {
                BOOST_REQUIRE_EQUAL(0xc0, msg.getHeader().all);
            });
            BOOST_REQUIRE_EQUAL(0, decoder.read(buf).value());
        }

        BOOST_AUTO_TEST_CASE(testPingRespEncode) {
            ArrayBuffer<64> buf;
            buf.append(rawPingResp, sizeof(rawPingResp));

            MQTTDecoder decoder;
            decoder.onPong([](const PingRespMessage &msg) {
                BOOST_REQUIRE_EQUAL(0xd0, msg.getHeader().all);
            });
            BOOST_REQUIRE_EQUAL(0, decoder.read(buf).value());
        }

    BOOST_AUTO_TEST_SUITE_END()
}