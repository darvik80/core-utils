//
// Created by Kishchenko Ivan on 07/06/2022.
//
#include "DnsResolver.h"

namespace network {
    DnsResolver::DnsResolver(boost::asio::io_service &service, std::string_view host, uint16_t port) : _service(service){
        boost::asio::ip::tcp::resolver::query query(host.data(), std::to_string(port));
        boost::asio::ip::tcp::resolver resolver(service);

        _result = resolver.resolve(query);
        _iter = _result.begin();
    }

    DnsResolver::DnsResolver(boost::asio::io_service &service) : _service(service) {
    }

    void DnsResolver::resolve(std::string_view host, uint16_t port) {
        boost::asio::ip::tcp::resolver::query query(host.data(), std::to_string(port));
        boost::asio::ip::tcp::resolver resolver(_service);

        _result = resolver.resolve(query);
        _iter = _result.begin();
    }

    boost::asio::ip::tcp::resolver::endpoint_type DnsResolver::next() {
        _iter++;
        if (_iter == _result.end()) {
            _iter = _result.begin();
        }

        return _iter->endpoint();
    }

    std::size_t DnsResolver::size() {
        return _result.size();
    }
}