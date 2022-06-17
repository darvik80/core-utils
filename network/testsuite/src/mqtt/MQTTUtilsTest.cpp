//
// Created by Kishchenko Ivan on 15/06/2022.
//

#include "MQTTUtilsTest.h"
#include "network/mqtt/MQTTUtils.h"

namespace network::mqtt {
    BOOST_FIXTURE_TEST_SUITE(MQTTUtilsTest, MQTTUtilsFixture)

        BOOST_AUTO_TEST_CASE(testCompareTopic) {
            BOOST_REQUIRE_EQUAL(true, MQTTUtils::compareTopics("/mail/+/+", "/mail/test/hello"));
            BOOST_REQUIRE_EQUAL(true, MQTTUtils::compareTopics("/test/+/hello", "/test/test/hello"));
            BOOST_REQUIRE_EQUAL(true, MQTTUtils::compareTopics("/test/+/hello/", "/test/test/hello/"));
            BOOST_REQUIRE_EQUAL(true, MQTTUtils::compareTopics("/test/+/#", "/test/test/hello"));
            BOOST_REQUIRE_EQUAL(true, MQTTUtils::compareTopics("/test/test/#", "/test/test/hello"));
            BOOST_REQUIRE_EQUAL(true, MQTTUtils::compareTopics("/+/+/#", "/test/test/hello"));

            BOOST_REQUIRE_EQUAL(false, MQTTUtils::compareTopics("/test/hello/#", "/test/test/hello"));
            BOOST_REQUIRE_EQUAL(false, MQTTUtils::compareTopics("/test/+/#", "/mail/test/hello"));
        }

    BOOST_AUTO_TEST_SUITE_END()
}