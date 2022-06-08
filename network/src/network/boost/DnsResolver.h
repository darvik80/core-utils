//
// Created by Kishchenko Ivan on 07/06/2022.
//

#ifndef RPI_ROBOT_DNSRESOLVER_H
#define RPI_ROBOT_DNSRESOLVER_H

#include <boost/asio.hpp>

namespace network {

    class DnsResolver {
        boost::asio::io_service& _service;
        boost::asio::ip::tcp::resolver::results_type _result;
        boost::asio::ip::tcp::resolver::iterator _iter;
    public:
        DnsResolver(boost::asio::io_service &service, std::string_view host, uint16_t port);

        DnsResolver(boost::asio::io_service &service);

        void resolve(std::string_view host, uint16_t port);
        boost::asio::ip::tcp::resolver::endpoint_type next();
        std::size_t size();
    };

}

#endif //RPI_ROBOT_DNSRESOLVER_H
