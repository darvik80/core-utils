//
// Created by Ivan Kishchenko on 03.02.2022.
//

#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>
#include <iterator>
#include <system_error>
#include <iomanip>
#include <array>
#include <vector>
#include <algorithm>

namespace network {

    enum class IOFlag {
        le,
        be,
        variable
    };

    enum IoError {
        le,
        be,
    };

    typedef int varInt;

    class Buffer {
        uint8_t *_pointer{};
        size_t _capacity{};

    protected:
        std::size_t _pos{};
    protected:
        void update(uint8_t *pointer, size_t size) {
            if (size > _capacity) {
                _pointer = pointer, _capacity = size;
            }
        }

    public:
        Buffer()
                : _pointer(nullptr), _capacity(0), _pos(0) {}

        Buffer(uint8_t *pointer, size_t size)
                : _pointer(pointer), _capacity(size), _pos(size) {}

        virtual std::error_code append(const uint8_t *data, size_t size) {
            if (available() < size) {
                return std::make_error_code(std::errc::message_size);
            }

            std::memcpy(_pointer + _pos, data, size);
            _pos += size;

            return {};
        }

        [[nodiscard]] const uint8_t *data() const {
            return _pointer;
        }

        [[nodiscard]] std::size_t capacity() const {
            return _capacity;
        };

        [[nodiscard]] virtual size_t size() const {
            return _pos;
        }

        [[nodiscard]] virtual size_t available() const {
            return capacity() - _pos;
        }

        void consume() {
            consume(_pos);
        }

        void consume(size_t size) {
            if (size > _pos) {
                size = _pos;
            }

            std::memcpy(_pointer, _pointer + size, _pos - size);
            _pos -= size;
        }

    };

    template<size_t Size>
    class ArrayBuffer : public Buffer {
        std::array<uint8_t, Size> _data{};
    public:
        ArrayBuffer() {
            update(_data.data(), _data.size());
        }
    };

    class VectorBuffer : public Buffer {
        std::vector<uint8_t> _data;
    public:
        std::error_code append(const uint8_t *data, size_t size) override {
            if (available() < size) {
                _data.resize(_data.size() + std::max(size, (size_t) 256));
                update(_data.data(), _data.size());
            }
            return Buffer::append(data, size);
        }
    };

    template<typename T>
    constexpr T reverse(T value) noexcept {
        char *ptr = reinterpret_cast<char *>(&value);
        std::reverse(ptr, ptr + sizeof(T));

        return value;
    }

    template<class T>
    struct SetFill {
        T _fill;
        size_t _size;
    public:
        SetFill(T fill, size_t size)
                : _fill(fill), _size(size) {}

    };

    struct Ignore {
        size_t _size;
    public:
        explicit Ignore(size_t size)
                : _size(size) {}
    };

    class Writer {
        IOFlag _flag{IOFlag::be};
        Buffer &_buf;
        std::error_code _code;
    public:
        explicit Writer(Buffer &buf)
                : _buf(buf) {}

        bool operator!() const {
            return _code.value() != 0;
        }

        size_t available() {
            return _buf.available();
        }

        std::error_code status() {
            return _code;
        }

        std::error_code write(const uint8_t *data, size_t size) {
            if (_code) {
                return _code;
            }

            if (auto err = _buf.append(data, size); err) {
                _code = err;
            }

            return _code;
        }

        template<class T>
        std::error_code write(IOFlag flag, T val) {
            if (flag == IOFlag::variable) {
                do {
                    uint8_t encoded = val % 0x80;
                    val /= 0x80;
                    if (val > 0) {
                        encoded |= 0x80;
                    }

                    if (auto err = write(&encoded, sizeof(uint8_t)); err) {
                        return err;
                    }
                } while (val > 0);
                return {};
            } else if (flag == IOFlag::be) {
                val = reverse(val);
            }

            return write((const uint8_t *) &val, sizeof(val));
        }

        friend Writer &operator<<(Writer &out, IOFlag val) {
            out._flag = val;
            return out;
        }


        friend Writer &operator<<(Writer &out, Buffer &val) {
            out.write(val.data(), val.size());
            return out;
        }

        friend Writer &operator<<(Writer &out, std::string_view val) {
            if (out._code) {
                return out;
            }

            if (auto err = out._buf.append((const uint8_t *) val.data(), val.size()); err) {
                out._code = err;
            }

            return out;
        }

        friend Writer &operator<<(Writer &out, const std::string &val) {
            out.write((const uint8_t *) val.data(), val.size());

            return out;
        }

        friend Writer &operator<<(Writer &out, const std::vector<uint8_t> &val) {
            out.write((const uint8_t *) val.data(), val.size());

            return out;
        }

        friend Writer &operator<<(Writer &out, const char *val) {
            out.write((const uint8_t *) val, std::strlen(val));

            return out;
        }

        friend Writer &operator<<(Writer &out, uint8_t val) {
            out.write(&val, sizeof(uint8_t));
            return out;
        }

        template<class T>
        friend Writer &operator<<(Writer &out, T val) {
            if constexpr (std::is_base_of<Buffer, T>::value) {
                out.write(val.data(), val.size());
            } else {
                out.write(out._flag, val);
            }
            return out;
        }

        template<class T>
        friend Writer &operator<<(Writer &out, SetFill<T> val) {
            if (out._code) {
                return out;
            }

            for (size_t idx = 0; idx < val._size; idx++) {
                if (auto err = out.write(&val._fill, sizeof(T)); err) {
                    out._code = err;
                    return out;
                }
            }

            return out;
        }
    };


    class Reader {
        IOFlag _flag{IOFlag::be};
        Buffer &_buf;
        std::error_code _code{};

        size_t _expectingSize{0};
        size_t _pos{};

    private:
        const uint8_t *data() {
            return _buf.data() + _pos;
        }

    public:
        explicit Reader(Buffer &buf)
                : _buf(buf) {}

        std::error_code status() {
            return _code;
        }

        size_t available() {
            return _buf.size() - _pos;
        }

        bool operator!() const {
            return _code.value() != 0;
        }

        virtual int read() {
            if (!available()) {
                return -1;
            }

            return _buf.data()[_pos++];
        }

        std::error_code read(uint8_t *ptr, size_t size) {
            if (available() < size) {
                _code = std::make_error_code(std::errc::message_size);
            } else {
                std::memcpy(ptr, data(), size);

                _pos += size;
            }

            return _code;
        }

        std::error_code read(size_t size, std::string &str) {
            str.resize(size);
            return read((uint8_t *) str.data(), size);
        }

        template<class T>
        std::error_code read(IOFlag flag, T &val) {
            if (flag == IOFlag::variable) {
                int multiplier = 1;
                int result = 0;

                uint8_t encoded = 0;
                do {
                    if (encoded = read(); status()) {
                        return status();
                    }
                    result += (encoded & 0x7F) * multiplier;
                    if (multiplier > 0x80 * 0x80 * 0x80) {
                        return std::make_error_code(std::errc::message_size);
                    }
                    multiplier *= 0x80;
                } while ((encoded & 0x80) != 0);

                val = result;
                return {};
            }

            const auto valSize = sizeof(val);
            if (auto err = read((uint8_t *) &val, valSize); err) {
                return err;
            }

            if (flag == IOFlag::be) {
                val = reverse(val);
            }

            return {};
        }

        std::error_code read(varInt &val) {
            int multiplier = 1;
            int result = 0;

            uint8_t encoded = 0;
            do {
                if (encoded = read(); status()) {
                    return status();
                }
                result += (encoded & 0x7F) * multiplier;
                if (multiplier > 0x80 * 0x80 * 0x80) {
                    return std::make_error_code(std::errc::message_size);
                }
                multiplier *= 0x80;
            } while ((encoded & 0x80) != 0);

            return {};
        }

        friend Reader &operator<<(Reader &inc, IOFlag val) {
            inc._flag = val;
            return inc;
        }

        template<class T>
        friend Reader &operator>>(Reader &inc, T &val) {
            inc.read(inc._flag, val);
            return inc;
        }

        friend Reader &operator<<(Reader &inc, size_t val) {
            inc._expectingSize = val;
            return inc;
        }

        friend Reader &operator>>(Reader &inc, std::string &val) {
            if (inc._expectingSize > inc.available()) {
                inc._code = std::make_error_code(std::errc::message_size);
            } else {
                val.resize(inc._expectingSize);
                inc.read((uint8_t *) val.data(), val.size());
            }

            return inc;
        }

        friend Reader &operator>>(Reader &inc, std::vector<uint8_t> &val) {
            if (inc._expectingSize > inc.available()) {
                inc._code = std::make_error_code(std::errc::message_size);
            } else {
                val.resize(inc._expectingSize);
                inc.read((uint8_t *) val.data(), val.size());
            }

            return inc;
        }

        friend Reader &operator<<(Reader &inc, Ignore val) {
            if (!inc._code) {
                uint8_t ignore;
                for (size_t idx = 0; idx < val._size; idx++) {
                    if (auto err = inc.read(&ignore, sizeof(uint8_t)); err) {
                        return inc;
                    }
                }
            }

            return inc;
        }

        size_t consumed() {
            return _pos;
        }
    };
}