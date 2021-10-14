//
// Created by Ivan Kishchenko on 10.10.2021.
//

#include <vector>
#include "network/Handler.h"
#include "network/handler/NetworkLogger.h"
#include "network/boost/AsyncTcpServer.h"

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

class ByteStream : public MessageHandler<ByteBuf> {
public:
    void handleActive() override {
        network::log::info("0. onActive");
        MessageHandler<ByteBuf>::handleActive();
    }

    void handleInactive() override {
        network::log::info("0. onInactive");
        MessageHandler<ByteBuf>::handleActive();
    }

    void handleRead(const ByteBuf &event) override {
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
    logger::LoggingProperties logProps;
    logProps.level = "info";
    logger::setup(logProps);

    boost::asio::io_service service;
    AsyncTcpServer server(service, [](const std::shared_ptr<Channel>& channel) {
        link(
                channel,
                std::make_shared<NetworkLogger>(),
                std::make_shared<ByteStream>()
        );
    });
    server.bind(8000);
    service.run();
    server.shutdown();

    return 0;
}

