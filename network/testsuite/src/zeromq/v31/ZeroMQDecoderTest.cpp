//
// Created by Ivan Kishchenko on 05.02.2022.
//

#include "ZeroMQDecoderTest.h"

//
// Created by Ivan Kishchenko on 05.02.2022.
//

#include "ZeroMQEncoderTest.h"
#include "network/zeromq/v31/ZeroMQDecoder.h"

namespace network::zeromq::v31 {

    BOOST_FIXTURE_TEST_SUITE(ZeroMQDecoderTest, ZeroMQDecoderFixture)

        BOOST_AUTO_TEST_CASE(testCmdDecoder) {
            uint8_t raw[] = {
                    0x04, 0x19, 0x05, 0x52,
                    0x45, 0x41, 0x44, 0x59,
                    0x0b, 0x53, 0x6f, 0x63,
                    0x6b, 0x65, 0x74, 0x2d,
                    0x54, 0x79, 0x70, 0x65,
                    0x00, 0x00, 0x00, 0x03,
                    0x50, 0x55, 0x42
            };

            ArrayBuffer<64> buf;
            buf.append(raw, sizeof(raw));

            ZeroMQDecoder decoder;
            decoder.onCommand([](ZeroMQCommand &cmd) {
                BOOST_REQUIRE_EQUAL(ZERO_MQ_CMD_READY, cmd.getName().c_str());
                BOOST_REQUIRE_EQUAL(ZERO_MQ_SOCKET_TYPE_PUB, cmd.props[ZERO_MQ_PROP_SOCKET_TYPE]);
            });
            BOOST_REQUIRE_EQUAL(0, decoder.read(buf).value());
        }

        BOOST_AUTO_TEST_CASE(testMsgDecoder) {
            uint8_t raw[] = {
                    0x01, 0x05, 0x48, 0x65,
                    0x6c, 0x6c, 0x6f, 0x00,
                    0x05, 0x57, 0x6f, 0x72,
                    0x6c, 0x64
            };

            ArrayBuffer<64> buf;
            buf.append(raw, sizeof(raw));

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