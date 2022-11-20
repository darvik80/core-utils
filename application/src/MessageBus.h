//
// Created by Ivan Kishchenko on 20.11.2022.
//


#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include "BaseService.h"

typedef boost::asio::io_service IOService;
typedef boost::asio::deadline_timer IOTimer;

class IMessage {
public:
    typedef std::shared_ptr<IMessage> Ptr;

    virtual ~IMessage() = default;
};

class IMessageHandler {
public:
    typedef std::shared_ptr<IMessageHandler> Ptr;

    virtual void onMessage(IMessage::Ptr msg) = 0;

    virtual ~IMessageHandler() = default;
};

class IScheduleStrategy {
public:
    virtual void apply(IOService &service, IMessageHandler::Ptr handler, IMessage::Ptr msg) = 0;

    virtual ~IScheduleStrategy() = default;
};

class IMessageProducer {
public:
    virtual void send(const IMessage &msg) = 0;

    virtual void post(const IMessage::Ptr &msg) = 0;

    virtual void schedule(const IMessage::Ptr &msg, IScheduleStrategy &&strategy) = 0;
};


class Timer : std::enable_shared_from_this<Timer> {
    std::shared_ptr<IOTimer> _timer;
public:
    typedef std::function<void()> Handler;
    typedef boost::posix_time::time_duration Duration;

    Timer() = default;

    explicit Timer(IOService &service)
            : _timer(std::make_unique<IOTimer>(service)) {}

    explicit Timer(std::shared_ptr<IOTimer> &timer)
            : _timer(std::move(timer)) {
    }

    explicit Timer(const std::weak_ptr<IOTimer> &timer)
            : _timer(timer) {
    }

    Timer(Timer &other)
            : _timer(std::move(other._timer)) {

    }

    Timer &operator=(const Timer &other) {
        if (&other == this) {
            return *this;
        }
        _timer = other._timer;
        return *this;
    }

    ~Timer() {
        cancel();
    }

public:
    void scheduleOnce(const Duration &duration, const Handler &fn) {
        _timer->expires_from_now(duration);
        _timer->async_wait([fn](const boost::system::error_code &ec) {
            if (!ec) {
                fn();
            }
        });
    }

    void scheduleAtFixedRate(const Duration &duration, const Handler &fn) {
        auto fnSelf = [timer = _timer, duration, fn](auto &selfRef) -> void {
            timer->expires_at(timer->expires_at() + duration);
            timer->async_wait([fn, selfRef](const boost::system::error_code &ec) {
                if (!ec) {
                    fn();
                    selfRef(selfRef);
                }
            });
        };

        scheduleOnce(duration, [fnSelf]() {
            fnSelf(fnSelf);
        });
    }

    void scheduleWithFixedDelay(const Duration &duration, const Handler &fn) {
        auto self = shared_from_this();
        _timer->expires_from_now(duration);
        _timer->async_wait([fn, duration, self](const boost::system::error_code &ec) {
            if (!ec) {
                fn();
                self->scheduleWithFixedDelay(duration, fn);
            }
        });
    }


    bool cancel() {
        if (_timer) {
            return _timer->cancel() > 0;
        }
        return false;
    }
};

class PostStrategy : public IScheduleStrategy {
public:
    PostStrategy() = default;

    void apply(IOService &service, IMessageHandler::Ptr handler, IMessage::Ptr msg) override {
        boost::asio::post(service, [handler, msg]() {
            handler->onMessage(msg);
        });
    }
};

class OnceDelayStrategy : public IScheduleStrategy {
    Timer::Duration _delay;
public:
    explicit OnceDelayStrategy(const Timer::Duration &delay) : _delay(delay) {}

    void apply(IOService &service, IMessageHandler::Ptr handler, IMessage::Ptr msg) override {
        auto timer = std::make_shared<Timer>(service);
        timer->scheduleOnce(_delay, [handler, msg]() {
            handler->onMessage(msg);
        });
    }
};

class FixedDelayStrategy : public IScheduleStrategy {
    Timer::Duration _initDelay;
    Timer::Duration _period;
public:
    FixedDelayStrategy(const Timer::Duration &initDelay, const Timer::Duration &period) : _initDelay(initDelay),
                                                                                          _period(period) {}

    void apply(IOService &service, IMessageHandler::Ptr handler, IMessage::Ptr msg) override {
        auto timer = std::make_shared<Timer>(service);
        if (_initDelay.is_positive()) {
            timer->scheduleOnce(_initDelay, [period = _period, timer, handler, msg]() {
                handler->onMessage(msg);
                timer->scheduleWithFixedDelay(period, [handler, msg]() {
                    handler->onMessage(msg);
                });
            });
        } else {
            handler->onMessage(msg);
            timer->scheduleWithFixedDelay(_period, [handler, msg]() {
                handler->onMessage(msg);
            });
        }
    }
};

class FixedRateStrategy : public IScheduleStrategy {
    Timer::Duration _initDelay;
    Timer::Duration _period;
public:
    FixedRateStrategy(const Timer::Duration &initDelay, const Timer::Duration &period) : _initDelay(initDelay),
                                                                                         _period(period) {}

    void apply(IOService &service, IMessageHandler::Ptr handler, IMessage::Ptr msg) override {
        auto timer = std::make_shared<Timer>(service);
        if (_initDelay.is_positive()) {
            timer->scheduleOnce(_initDelay, [period = _period, timer, handler, msg]() {
                handler->onMessage(msg);
                timer->scheduleAtFixedRate(period, [handler, msg]() {
                    handler->onMessage(msg);
                });
            });
        } else {
            handler->onMessage(msg);
            timer->scheduleAtFixedRate(_period, [handler, msg]() {
                handler->onMessage(msg);
            });
        }
    }
};

template<class T>
class TMessageHandler : virtual public IMessageHandler {
public:
    virtual void onMessage(const T &event) = 0;
};

class MessageBus : public IMessageProducer, public IMessageHandler, std::enable_shared_from_this<MessageBus> {
    IOService &_service;

    typedef boost::signals2::signal<void(const IMessage &)> Signal;
    typedef std::unordered_map<std::type_index, Signal> Signals;
    Signals _signals;
private:
    void onMessage(IMessage::Ptr msg) override {
        send(*msg);
    }

public:
    explicit MessageBus(IOService &service) : _service(service) {}

    void send(const IMessage &msg) override {
        if (auto iter = _signals.find(typeid(msg)); iter != _signals.end()) {
            iter->second(msg);
        }
    }

    void post(const IMessage::Ptr &ptr) override {
        schedule(ptr, PostStrategy{});
    }

    void schedule(const IMessage::Ptr &msg, IScheduleStrategy &&strategy) override {
        strategy.apply(_service, shared_from_this(), msg);
    }

    template<class M>
    void subscribe(const IMessageHandler::Ptr &handler) {
        auto *channel = dynamic_cast<TMessageHandler<M> *>(handler.get());
        if (!channel) {
            throw std::invalid_argument(std::string("[MsgBus] handler not support event ") + typeid(M).name());
        }

        _signals[typeid(M)].connect(boost::signals2::signal<void(const IMessage &)>::slot_type(
                [channel](const IMessage &msg) {
                    const auto *real = static_cast<const M *>(&msg);
                    if (real) {
                        channel->onMessage(*real);
                    }
                }
        ).track_foreign(handler));
    }

    template<class M>
    void subscribe(const std::function<bool(const M &)> &callback) {
        _signals[typeid(M)].connect_extended(
                [callback](const boost::signals2::connection &conn, const IMessage &event) {
                    const auto *real = static_cast<const M *>(&event);
                    if (!callback(*real)) {
                        conn.disconnect();
                    }
                }
        );
    }
};

LOG_COMPONENT_SETUP(bus, bus_logger)

class MessageBusService : public BaseService {
public:
    MessageBusService() : BaseService(bus_logger::get()) {}

    int order() override {
        return INT_MAX-1;
    }

    const char *name() override {
        return "message-bus";
    }
};