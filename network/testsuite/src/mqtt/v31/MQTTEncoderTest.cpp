//
// Created by Ivan Kishchenko on 05.02.2022.
//

#include "MQTTEncoderTest.h"
#include "MQTTTestData.h"
#include "network/mqtt/v31/MQTTEncoder.h"

namespace network::mqtt::v31 {

    BOOST_FIXTURE_TEST_SUITE(MQTTEncoderV31Test, MQTTEncoderV31Fixture)

        BOOST_AUTO_TEST_CASE(testConnectEncode) {
            ConnectMessage msg;
            msg.setClientId("paho/DDE4DDAF4108D3E363");
            msg.setKeepAlive(5);

            ArrayBuffer<64> buf;
            MQTTEncoder encoder;

            BOOST_REQUIRE_EQUAL(0, encoder.write(buf, msg).value());
            BOOST_REQUIRE_EQUAL(39, buf.size());
            BOOST_REQUIRE_EQUAL_COLLECTIONS(&rawConnect[0], &rawConnect[38], &buf.data()[0], &buf.data()[38]);
        }

        BOOST_AUTO_TEST_CASE(testConnAckEncode) {
            ConnAckMessage msg;
            msg.setReasonCode(0);
            msg.setFlags(0);

            ArrayBuffer<64> buf;
            MQTTEncoder encoder;

            BOOST_REQUIRE_EQUAL(0, encoder.write(buf, msg).value());
            BOOST_REQUIRE_EQUAL(4, buf.size());
            BOOST_REQUIRE_EQUAL_COLLECTIONS(&rawConnAck[0], &rawConnAck[3], &buf.data()[0], &buf.data()[3]);
        }

        BOOST_AUTO_TEST_CASE(testPingReqEncode) {
            PingReqMessage msg;

            ArrayBuffer<64> buf;
            MQTTEncoder encoder;

            BOOST_REQUIRE_EQUAL(0, encoder.write(buf, msg).value());
            BOOST_REQUIRE_EQUAL(2, buf.size());
            BOOST_REQUIRE_EQUAL_COLLECTIONS(&rawPingReq[0], &rawPingReq[1], &buf.data()[0], &buf.data()[1]);
        }

        BOOST_AUTO_TEST_CASE(testPingRespEncode) {
            PingRespMessage msg;

            ArrayBuffer<64> buf;
            MQTTEncoder encoder;

            BOOST_REQUIRE_EQUAL(0, encoder.write(buf, msg).value());
            BOOST_REQUIRE_EQUAL(2, buf.size());
            BOOST_REQUIRE_EQUAL_COLLECTIONS(&rawPingResp[0], &rawPingResp[1], &buf.data()[0], &buf.data()[1]);
        }

    BOOST_AUTO_TEST_SUITE_END()
}