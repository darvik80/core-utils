//
// Created by Kishchenko Ivan on 07/06/2022.
//

#ifndef RPI_ROBOT_DNSRESOLVER_H
#define RPI_ROBOT_DNSRESOLVER_H

#include <boost/asio.hpp>

namespace network {
    namespace asio = boost::asio;
    namespace ip = boost::asio::ip;

    class DnsResolver {

        asio::io_service &_service;
        ip::tcp::resolver::results_type _result;
        ip::tcp::resolver::iterator _iter;
    public:
        DnsResolver(asio::io_service &service, std::string_view host, uint16_t port);

        explicit DnsResolver(asio::io_service &service);

        void resolve(std::string_view host, uint16_t port);

        ip::tcp::resolver::endpoint_type next();

        std::size_t size();
    };

}

#endif //RPI_ROBOT_DNSRESOLVER_H
