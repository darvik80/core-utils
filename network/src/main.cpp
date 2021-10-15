//
// Created by Ivan Kishchenko on 10.10.2021.
//

#include <vector>
#include "network/Handler.h"
#include "network/handler/NetworkLogger.h"
#include "network/boost/AsyncTcpServer.h"
#include "network/boost/AsyncTcpClient.h"
#include "network/zeromq/ZeroMQCodec.h"
#include "network/zeromq/ZeroMQHandler.h"

using namespace network;

const char* ws = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string& rtrim(std::string& s, const char* t = ws)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t = ws)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trim(std::string& s, const char* t = ws)
{
    return ltrim(rtrim(s, t), t);
}

class ByteStream : public MessageHandler<ByteBuffer> {
public:
    void handleActive() override {
        network::log::info("0. onActive");
        MessageHandler<ByteBuffer>::handleActive();
    }

    void handleInactive() override {
        network::log::info("0. onInactive");
        MessageHandler<ByteBuffer>::handleActive();
    }

    void handleRead(ByteBuffer &event) override {
        std::string msg(event.data(), event.size());
        trim(msg);
        network::log::info("0. handle: {}", msg);
        if (msg == "quit") {
            shutdown();
        } else {
            write(event);
        }
    }

    ~ByteStream() override {
        network::log::info("~ByteStream");
    }

};


int main(int argc, char *argv[]) {
    uint16_t port = 8000;
    logger::LoggingProperties logProps;
    logProps.level = "info";
    logger::setup(logProps);

    boost::asio::io_service service;
    auto subscriber = std::make_shared<zeromq::ZeroMQSubscriber>();
    subscriber->subscribe("joystick", [](std::string_view topic, std::string_view data) {
        logger::info("sub: {}:{}", topic, data);
    });
    AsyncTcpServer server(service, [subscriber](const std::shared_ptr<AsyncChannel>& channel) {
        link(
                channel,
                std::make_shared<handler::NetworkLogger>(),
                std::make_shared<zeromq::ZeroMQCodec>(),
                subscriber
        );
    });
    server.bind(port);

    auto producer = std::make_shared<zeromq::ZeroMQPublisher>();
    AsyncTcpClient client(service, [producer](const std::shared_ptr<AsyncChannel>& channel) {
        link(
                channel,
                std::make_shared<handler::NetworkLogger>(),
                std::make_shared<zeromq::ZeroMQCodec>(),
                producer
        );
    });
    client.connect("127.0.0.1", port);

    boost::asio::deadline_timer deadline(service);
    deadline.expires_from_now(boost::posix_time::seconds(10));
    deadline.async_wait([producer, &server](boost::system::error_code err) {
        if (!err) {
            producer->publish("joystick", "Hello World");
        }
    });

    service.run();
    client.shutdown();
    server.shutdown();

    return 0;
}

