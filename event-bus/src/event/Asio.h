//
// Created by Kishchenko Ivan on 21/11/2022.
//


#pragma once

#include <boost/asio.hpp>

namespace bus {
    typedef boost::asio::deadline_timer IOTimer;
    typedef boost::asio::io_service IOService;
}
