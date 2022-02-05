//
// Created by Ivan Kishchenko on 05.02.2022.
//

#include "ZeroMQDecoderTest.h"
#include "ZeroMQTestData.h"
#include "network/zeromq/v31/ZeroMQDecoder.h"

namespace network::zeromq::v31 {

    BOOST_FIXTURE_TEST_SUITE(ZeroMQDecoderV31Test, ZeroMQDecoderV31Fixture)

        BOOST_AUTO_TEST_CASE(testCmdReadyDecoder) {
            ArrayBuffer<64> buf;
            buf.append(rawCmdReady, sizeof(rawCmdReady));

            ZeroMQDecoder decoder;
            decoder.onCommand([](ZeroMQCommand &cmd) {
                BOOST_REQUIRE_EQUAL(ZERO_MQ_CMD_READY, cmd.getName().c_str());
                BOOST_REQUIRE_EQUAL(ZERO_MQ_SOCKET_TYPE_PUB, cmd.props[ZERO_MQ_PROP_SOCKET_TYPE]);
            });
            BOOST_REQUIRE_EQUAL(0, decoder.read(buf).value());
        }

        BOOST_AUTO_TEST_CASE(testCmdSubDecoder) {

            ArrayBuffer<64> buf;
            buf.append(rawCmdSub, sizeof(rawCmdSub));

            ZeroMQDecoder decoder;
            decoder.onCommand([](ZeroMQCommand &cmd) {
                BOOST_REQUIRE_EQUAL(ZERO_MQ_CMD_SUBSCRIBE, cmd.getName().c_str());
                BOOST_REQUIRE_EQUAL("test", cmd.props[ZERO_MQ_PROP_SUBSCRIPTION]);
            });
            BOOST_REQUIRE_EQUAL(0, decoder.read(buf).value());
        }

        BOOST_AUTO_TEST_CASE(testMsgDecoder) {
            ArrayBuffer<64> buf;
            buf.append(rawMsg, sizeof(rawMsg));

            ZeroMQDecoder decoder;
            decoder.onMessage([](ZeroMQMessage &msg) {
                BOOST_REQUIRE_EQUAL("Hello", msg.data[0].c_str());
                BOOST_REQUIRE_EQUAL("World", msg.data[1].c_str());
            });
            BOOST_REQUIRE_EQUAL(0, decoder.read(buf).value());
            BOOST_REQUIRE_EQUAL(0, decoder.read(buf).value());
            BOOST_REQUIRE_EQUAL(EMSGSIZE, decoder.read(buf).value());
        }

    BOOST_AUTO_TEST_SUITE_END()

}