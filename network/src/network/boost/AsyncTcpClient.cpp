//
// Created by Ivan Kishchenko on 15.10.2021.
//

#include "AsyncTcpClient.h"
#include "network/Logging.h"

namespace network {

    using namespace boost;

    AsyncTcpClient::AsyncTcpClient(asio::io_service &service, const onConnectCallback &callback)
            : _service(service), _resolver(service), _deadline(service), _callback(callback) {

    }

    void AsyncTcpClient::doConnect(asio::io_service &service, asio::ip::tcp::resolver::results_type endpoints, asio::ip::tcp::resolver::results_type::iterator iter) {
        auto iterNext = iter;
        iterNext++;
        if (iter != asio::ip::tcp::resolver::results_type::iterator()) {
            auto socket = std::make_shared<asio::ip::tcp::socket>(service);
            socket->async_connect(
                    iter->endpoint(),
                    [this, &service, endpoints, socket, iterNext](boost::system::error_code err) {
                        if (err) {
                            doConnect(service, endpoints, iterNext);
                        } else {
                            std::shared_ptr<AsyncChannel> channel(new AsyncChannel(std::move(*socket)), [this, &service, endpoints](AsyncChannel *chan) {
                                doConnect(service, endpoints, endpoints.begin());

                                network::log::info("destroy channel");
                                delete chan;
                            });
                            _callback(channel);
                            channel->start();
                        }
                    }
            );
        } else {
            _deadline.expires_from_now(posix_time::seconds(10));
            _deadline.async_wait([this, &service, endpoints](boost::system::error_code err) {
                if (!err) {
                    doConnect(service, endpoints, endpoints.begin());
                }
            });
        }
    }

    void AsyncTcpClient::connect(std::string_view host, uint16_t port) {
        asio::ip::tcp::resolver::query query(host.data(), std::to_string(port));

        auto endpoints = _resolver.resolve(query);
        doConnect(_service, endpoints, endpoints.begin());
    }

    void AsyncTcpClient::shutdown() {
        _deadline.cancel();
    }
}