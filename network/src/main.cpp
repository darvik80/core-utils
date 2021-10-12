//
// Created by Ivan Kishchenko on 10.10.2021.
//

#include <vector>
#include "network/Handler.h"

struct Greeting {
    std::string greeting;
};

struct Body {
    std::string body;
};

class ByteStream : public MessageHandler<std::string, Greeting> {
public:
    void handleActive() override {
        network::log::info("0. onActive");
    }

    void handleInactive() override {
        network::log::info("0. onInactive");
    }

    void handleRead(const std::string &event) override {
        network::log::info("0. handle: {}", event);
        Greeting greeting{event};
        trigger(greeting);
    }

    void handleWrite(const Greeting &event) override {
        network::log::info("0. write: {}", event.greeting);
    }

    ~ByteStream() override {
        network::log::info("~ByteStream");
    }

};

class GreetingHandler : public MessageHandler<Greeting, Body> {
public:
    void handleRead(const Greeting &event) override {
        network::log::info("1. read greeting: {}", event.greeting);
        Body body;
        body.body = "body:" + event.greeting;
        trigger(body);
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

template<typename A, typename B>
auto link(std::shared_ptr<A> a, std::shared_ptr<B> b) {
    a->setNext(b);
    b->setPrev(a);

    return a;
}

template<typename A, typename B, typename C>
auto link(std::shared_ptr<A> a, std::shared_ptr<B> b, std::shared_ptr<C> c) {
    a->setNext(b);
    b->setPrev(a);

    b->setNext(c);
    c->setPrev(b);

    return a;
}

template<typename A, typename B, typename C, typename D>
auto link(A a, B b, C c, D d) {
    a->setNext(b);
    b->setPrev(a);

    b->setNext(c);
    c->setPrev(b);

    c->setNext(d);
    d->setPrev(c);

    return a;
}

template<typename A, typename B, typename C, typename D, typename E>
auto link(A a, B b, C c, D d, E e) {
    a->setNext(b);
    b->setPrev(a);

    b->setNext(c);
    c->setPrev(b);

    c->setNext(d);
    d->setPrev(c);

    d->setNext(e);
    e->setPrev(d);

    return a;
}

int main(int argc, char *argv[]) {
    logger::LoggingProperties logProps;
    logProps.level = "info";
    logger::setup(logProps);

    auto root = link(
            std::make_shared<ByteStream>(),
            std::make_shared<GreetingHandler>(),
            std::make_shared<BodyHandler>()
    );
    root->handleActive();
    root->handleRead("test");
    root->handleInactive();

    return 0;
}

