//
// Created by Ivan Kishchenko on 05.02.2022.
//

#include "DnsResolverTest.h"
#include "network/boost/DnsResolver.h"

namespace network {

    BOOST_FIXTURE_TEST_SUITE(DnsResolverTest, DnsResolverFixture)

        BOOST_AUTO_TEST_CASE(testConstuctor) {
            boost::asio::io_service service;
            DnsResolver resolver(service, "iot.crearts.xyz", 1883);

            BOOST_REQUIRE_EQUAL(1, resolver.size());
            BOOST_REQUIRE_EQUAL("62.84.123.79", resolver.next().address().to_string());
        }

        BOOST_AUTO_TEST_CASE(testResolve) {
            boost::asio::io_service service;
            DnsResolver resolver(service);

            resolver.resolve("iot.crearts.xyz", 1883);
            BOOST_REQUIRE_EQUAL(1, resolver.size());
            BOOST_REQUIRE_EQUAL("62.84.123.79", resolver.next().address().to_string());
        }

    BOOST_AUTO_TEST_SUITE_END()

}