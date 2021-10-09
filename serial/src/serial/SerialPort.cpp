//
// Created by Ivan Kishchenko on 15.05.2021.
//

#include "SerialPort.h"
#include "SerialLogger.h"

#include <boost/crc.hpp>

using namespace boost;

BoostSerialPort::BoostSerialPort(boost::asio::io_service &service, const SerialPortProperties& props)
        : _serial(service), _timer(service), _props(props) {

    open();
    setTimer(posix_time::seconds{5}, [this]() { onIdle(); });

    asyncRead();
}

boost::future<void> BoostSerialPort::send(uint8_t msgId, const uint8_t *data, size_t size) {
    auto promise = std::make_shared<boost::promise<void>>();
    _out.sputc(MSG_MAGIC);
    _out.sputc((char) msgId);
    _out.sputc((char) size);

    if (size) {
        _out.sputn((const char *) data, (int) size);
    }
    boost::crc_16_type crc16;
    crc16.process_block(data, data + size);
    uint16_t ctrl = crc16.checksum();
    _out.sputn((const char *) &ctrl, 2);
    _out.sputc(MSG_MAGIC);

    asio::async_write(_serial, _out, [promise, this](const boost::system::error_code &ec, std::size_t size) {
        if (ec) {
            promise->set_exception(system::system_error(ec));
            onError(ec);
        } else {
            promise->set_value();
        }
    });

    return promise->get_future();
}

void BoostSerialPort::asyncRead() {
    if (!_serial.is_open()) {
        return;
    }

    _serial.async_read_some(
            boost::asio::buffer(_incBuf, SERIAL_PORT_READ_BUF_SIZE),
            [this](const boost::system::error_code &ec, size_t size) {
                cancelTimer();

                if (!_serial.is_open()) {
                    return;
                }

                if (ec) {
                    onError(ec);
                    setTimer(posix_time::seconds{5}, [this]() {
                        try {
                            open();
                            asyncRead();
                        } catch (std::exception& ex) {
                            serial::log::warning("can't reopen port: {}", ex.what());
                        }
                    });
                    return;
                }
                _inc.sputn(_incBuf, (int) size);

                serial::log::debug("inc before {}/{}", size, _inc.size());

                std::istream inc(&_inc);
                while (!inc.eof()) {
                    if (_recvState == IDLE) {
                        auto ch = inc.get();
                        if (ch == EOF) {
                            continue;
                        }
                        if (MSG_MAGIC != ch) {
                            continue;
                        }
                        _recvState = HEADER;
                    } else if (_recvState == HEADER) {
                        if (_inc.size() < 2) {
                            break;
                        }
                        _cmd = inc.get();
                        _len = inc.get();
                        _buffer.resize(_len);
                        _recvState = BODY;
                    } else if (_recvState == BODY) {
                        if (_len > 0) {
                            if (_inc.size() < _len) {
                                break;
                            }
                            inc.read((char *) _buffer.data(), _len);
                        }
                        _recvState = FOOTER;
                    } else if (_recvState == FOOTER) {
                        if (_inc.size() < 3) {
                            break;
                        }

                        uint16_t ctrl{0};
                        inc.read((char *) &ctrl, 2);
                        int magic = inc.get();

                        boost::crc_16_type crc16;
                        crc16.process_block(_buffer.data(), _buffer.data() + _buffer.size());
                        if (crc16.checksum() != ctrl || magic != MSG_MAGIC) {
                            _recvState = IDLE;
                            break;
                        }

                        onMessage( _cmd, _buffer.data(), _buffer.size());
                        _recvState = IDLE;
                    }
                }
                serial::log::debug("inc after {}", _inc.size());
                asyncRead();
            }
    );

    setTimer(boost::posix_time::seconds{5}, [this]() { onIdle(); });
}

std::string BoostSerialPort::deviceId() {
    return _props.port;
}

void BoostSerialPort::onConnect() {
    std::for_each(_callbacks.begin(), _callbacks.end(), [this](SerialPortCallback::Ptr callback) {
       callback->onConnect(*this);
    });
}

void BoostSerialPort::onDisconnect() {
    std::for_each(_callbacks.begin(), _callbacks.end(), [this](SerialPortCallback::Ptr callback) {
        callback->onDisconnect(*this);
    });
}

void BoostSerialPort::onIdle() {
    std::for_each(_callbacks.begin(), _callbacks.end(), [this](SerialPortCallback::Ptr callback) {
        callback->onIdle(*this);
    });

    setTimer(posix_time::seconds{5}, [this]() {
        onIdle();
    });
}

void BoostSerialPort::onError(const system::error_code &ec) {
    std::for_each(_callbacks.begin(), _callbacks.end(), [this, ec](SerialPortCallback::Ptr callback) {
        callback->onError(*this, ec);
    });
}

void BoostSerialPort::onMessage(uint8_t msgId, const uint8_t *data, size_t size) {
    std::for_each(_callbacks.begin(), _callbacks.end(), [this, msgId, data, size](SerialPortCallback::Ptr callback) {
        callback->onMessage(*this, msgId, data, size);
    });
}

void BoostSerialPort::open() {
    if (_serial.is_open()) {
        try {
            _serial.cancel();
            _serial.close();
        } catch (std::exception& ex) {
            serial::log::warning("can't close port {}, {}", _props.port, ex.what());
        }

        onDisconnect();
    }

    _serial.open(_props.port);
    _serial.set_option(boost::asio::serial_port_base::baud_rate(_props.baudRate));
    _serial.set_option(boost::asio::serial_port_base::character_size(8));
    _serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    _serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    _serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    onConnect();
}

void BoostSerialPort::setTimer(posix_time::time_duration duration, const std::function<void()>& fn) {
    _timer.expires_from_now(duration);
    _timer.async_wait([fn](const boost::system::error_code& ec) {
        if (!ec) {
            fn();
        }
    });
}

void BoostSerialPort::cancelTimer() {
    _timer.cancel();
}