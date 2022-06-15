//
// Created by Ivan Kishchenko on 05.02.2022.
//

#include "BufferTest.h"
#include "network/Buffer.h"

namespace network {

    BOOST_FIXTURE_TEST_SUITE(BufferTest, BufferFixture)

        BOOST_AUTO_TEST_CASE(testMainFunctions) {
            ArrayBuffer<8> buffer;
            uint8_t buf[] = {0x64, 0x65, 0x66, 0x67, 0x68};

            buffer.append(buf, sizeof(buf));
            BOOST_REQUIRE_EQUAL(5, buffer.size());

            // append only if we can
            buffer.append(buf, sizeof(buf));
            BOOST_REQUIRE_EQUAL(5, buffer.size());

            buffer.consume(5);
            BOOST_REQUIRE_EQUAL(0, buffer.size());

        }

    BOOST_AUTO_TEST_SUITE_END()

}