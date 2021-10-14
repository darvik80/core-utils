//
// Created by Ivan Kishchenko on 10.10.2021.
//

#include <vector>
#include "network/Handler.h"
#include "network/handler/NetworkLogger.h"
#include "network/boost/AsyncTcpServer.h"

struct Greeting {
    std::string greeting;
};

struct Body {
    std::string body;
};

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

class ByteStream : public MessageHandler<ByteBuf, Greeting> {
public:
    void handleActive() override {
        network::log::info("0. onActive");
        MessageHandler<ByteBuf, Greeting>::handleActive();
    }

    void handleInactive() override {
        network::log::info("0. onInactive");
        MessageHandler<ByteBuf, Greeting>::handleActive();
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
        //Greeting greeting{std::string(event.data(), event.size())};
        //trigger(greeting);
    }

    void handleWrite(const Greeting &event) override {
        network::log::info("0. write: {}", event.greeting);
        //ByteBuf buf(event.greeting.begin(), event.greeting.end());
        //write(buf);
    }

    ~ByteStream() override {
        network::log::info("~ByteStream");
    }

};

class GreetingHandler : public MessageHandler<Greeting, Body> {
public:
    void handleRead(const Greeting &event) override {
        network::log::info("1. read greeting: {}", event.greeting);
        trigger(Body{event.greeting});
    }

    void handleWrite(const Body &event) override {
        network::log::info("1. write greeting: {}", event.body);
        write(Greeting{"hello world"});
    }

    ~GreetingHandler() override {
        network::log::info("~GreetingHandler");
    }
};

class BodyHandler : public MessageHandler<Body, Greeting> {
public:

    void handleRead(const Body &event) override {
        network::log::info("2. body: {}", event.body);
        write(event);
    }

    ~BodyHandler() override {
        network::log::info("~BodyHandle");
    }
};


int main(int argc, char *argv[]) {
    logger::LoggingProperties logProps;
    logProps.level = "info";
    logger::setup(logProps);

    auto root = link(
            std::make_shared<NetworkLogger>(),
            std::make_shared<ByteStream>(),
            std::make_shared<GreetingHandler>(),
            std::make_shared<BodyHandler>()
    );
    root->handleActive();
    root->handleRead(ByteBuf({0x74, 0x65, 0x73, 0x74}));
    root->handleInactive();

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

