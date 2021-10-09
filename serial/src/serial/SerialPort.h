//
// Created by Ivan Kishchenko on 15.05.2021.
//

#ifndef CORE_UTILS_SERIALPORT_H
#define CORE_UTILS_SERIALPORT_H

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <set>
#include <memory>

#include <boost/thread/future.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "Protocol.h"

#define SERIAL_PORT_READ_BUF_SIZE 1024

struct SerialPortProperties {
    std::string port;
    uint baudRate;
};

class SerialPort {
public:
    virtual std::string deviceId() = 0;
    virtual boost::future<void> send(uint8_t msgId, const uint8_t *data, size_t size) = 0;
    virtual void onMessage(uint8_t msgId, const uint8_t *data, size_t size) = 0;
};

class SerialPortCallback {
public:
    typedef std::shared_ptr<SerialPortCallback> Ptr;
    typedef std::set<Ptr> SetPtr;
public:
    virtual void onConnect(SerialPort& port) = 0;
    virtual void onDisconnect(SerialPort& port) = 0;
    virtual void onIdle(SerialPort& port) = 0;
    virtual void onMessage(SerialPort& port, uint8_t msgId, const uint8_t *data, size_t size) = 0;
    virtual void onError(SerialPort& port, const boost::system::error_code& ec) = 0;
};

class BoostSerialPort : public SerialPort {
    boost::asio::deadline_timer _timer;
    boost::asio::serial_port _serial;
    SerialPortProperties _props;

    char _incBuf[SERIAL_PORT_READ_BUF_SIZE];
    boost::asio::streambuf _inc;
    boost::asio::streambuf _out;

    enum RecvState {
        IDLE,
        HEADER,
        BODY,
        FOOTER
    };

    RecvState _recvState{IDLE};
    int _cmd{0};
    int _len{0};
    std::vector<uint8_t> _buffer{UINT8_MAX};


    SerialPortCallback::SetPtr _callbacks;
public:
    BoostSerialPort(boost::asio::io_service& service, const SerialPortProperties&  props);

    std::string deviceId() override;
    boost::future<void> send(uint8_t msgId, const uint8_t *data, size_t size) override;
    void onMessage(uint8_t msgId, const uint8_t *data, size_t size) override;

    void addCallback(SerialPortCallback::Ptr callback) {
        _callbacks.emplace(callback);
    }

private:
    void open();
    void setTimer(boost::posix_time::time_duration duration, const std::function<void()>& fn);
    void cancelTimer();

private:
    void asyncRead();

    void onIdle();

    void onConnect();

    void onDisconnect();

    void onError(const boost::system::error_code &ec);

};


#endif //CORE_UTILS_SERIALPORT_H
