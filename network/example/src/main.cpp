//
// Created by Ivan Kishchenko on 10.10.2021.
//

#include <vector>
#include "network/Handler.h"
#include "network/handler/NetworkLogger.h"
#include "network/handler/IdleStateHandler.h"
#include "network/boost/AsyncTcpServer.h"
#include "network/boost/AsyncTcpClient.h"
#include "network/zeromq/ZeroMQCodec.h"
#include "network/zeromq/ZeroMQHandler.h"

#include "network/mqtt/MQTTCodec.h"
#include "network/mqtt/MQTTHandler.h"

using namespace network;
using namespace boost;

#include <logging/Logging.h>

LOG_COMPONENT_SETUP(app, app_logger);

void exampleZeroMQ(boost::asio::io_service &service) {
    uint16_t port = 5556;

    auto subscriber = std::make_shared<zeromq::ZeroMQSubscriber>();
    subscriber->subscribe("joystick", [](std::string_view topic, std::string_view data) {
        logger::info("sub: {}:{}", topic, data);
    });

    AsyncServer<SslSocket> server(
            service,
            [subscriber](const std::shared_ptr<AsyncChannel<SslSocket>> &channel) {
                link(
                        channel,
                        std::make_shared<handler::NetworkLogger>(),
                        std::make_shared<zeromq::ZeroMQCodec>(),
                        subscriber
                );
            },
            "/Users/darvik/server.pem",
            "/Users/darvik/server_key.pem"
    );
//    AsyncServer<TcpSocket> server(
//            service,
//            [subscriber](const std::shared_ptr<AsyncChannel<TcpSocket>> &channel) {
//                link(
//                        channel,
//                        std::make_shared<handler::NetworkLogger>(),
//                        std::make_shared<zeromq::ZeroMQCodec>(),
//                        subscriber
//                );
//            }
//    );

    server.bind(port);

    auto producer = std::make_shared<zeromq::CompositeProducer>();
    AsyncClient<SslSocket> client(
            service,
            [producer](const std::shared_ptr<AsyncChannel<SslSocket>> &channel) {
                link(
                        channel,
                        std::make_shared<handler::NetworkLogger>(),
                        std::make_shared<zeromq::ZeroMQCodec>(),
                        std::make_shared<zeromq::ZeroMQPublisher>(producer)
                );
            },
            "/Users/darvik/server.pem"
    );

//    AsyncClient<TcpSocket> client(
//            service,
//            [producer](const std::shared_ptr<AsyncChannel<TcpSocket>> &channel) {
//                link(
//                        channel,
//                        std::make_shared<handler::NetworkLogger>(),
//                        std::make_shared<zeromq::ZeroMQCodec>(),
//                        std::make_shared<zeromq::ZeroMQPublisher>(producer)
//                );
//            }
//    );

    client.connect("127.0.0.1", port);

    boost::asio::deadline_timer deadline(service);
    deadline.expires_from_now(boost::posix_time::seconds(10));
    deadline.async_wait([producer](boost::system::error_code err) {
        if (!err) {
            producer->publish("joystick", "Hello World");
            producer->publish("test", "Skipped message");
            producer->publish("joystick", "Second message");
        }
    });

    service.run();
}

void exampleMQTT(boost::asio::io_service &service) {
    uint16_t port = 1883;

    auto producer = std::make_shared<mqtt::MQTTPublisher>();
    AsyncClient<TcpSocket> client(
            service,
            [producer, &service](const std::shared_ptr<AsyncChannel<TcpSocket>> &channel) {
                link(
                        channel,
                        std::make_shared<handler::NetworkLogger>(),
                        std::make_shared<handler::IdleStateHandler>(service, posix_time::seconds(5), posix_time::seconds(5)),
                        std::make_shared<mqtt::MQTTCodec>(),
                        producer
                );
            },
            "/Users/darvik/server.pem"
    );

    client.connect("iot.crearts.xyz", port);

    boost::asio::deadline_timer deadline(service);
    deadline.expires_from_now(boost::posix_time::seconds(10));
    deadline.async_wait([producer](boost::system::error_code err) {
        if (!err) {
            producer->publish("v1/devices/me/telemetry", R"({ "status": "active" })");
            producer->publish("v1/devices/me/telemetry", R"({ "cpu-temp": "471000" })");
        }
    });

    service.run();
}

int main(int argc, char *argv[]) {
    logger::LoggingProperties logProps;
    logProps.level = "debug";
    logger::setup(logProps);

    boost::asio::io_service service;

    boost::asio::signal_set signals(service);
    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif

    signals.async_wait(
            [&service](boost::system::error_code ec, int signal) {
                if (!ec) {
                    service.stop();
                    app::log::info("service shutdown");
                }
            }
    );

    //exampleZeroMQ(service);

    exampleMQTT(service);

    return 0;
}

