//
// Created by Ivan Kishchenko on 05.09.2021.
//

#pragma once

#include <string>
#include <streambuf>
#include <array>
#include <cstring>

namespace network {

    template<class CharT = char>
    class ByteBuf : public std::basic_streambuf<CharT> {
    protected:
        ByteBuf() = default;

    public:
        using Base = std::basic_streambuf<CharT>;
    public:
        std::streamsize xsputn(const CharT *s, std::streamsize n) override {
            Base::setg(Base::pbase(), Base::pbase(), Base::pptr() + n);
            return Base::xsputn(s, n);
        }

    public:
        [[nodiscard]] std::size_t size() {
            return Base::pptr() - Base::pbase();
        }

        [[nodiscard]] std::size_t available() {
            return Base::epptr() - Base::pbase() - Base::in_avail();
        }

        const CharT *data() const {
            return Base::pbase();
        }

        bool append(const CharT *s, std::streamsize n) {
            return Base::sputn(s, n) == n;
        }

        bool consume(std::size_t needConsume) {
            if (needConsume > size()) {
                return false;
            }

            size_t avail = size() - needConsume;
            std::memcpy(Base::pbase(), Base::pbase() + needConsume, avail);
            Base::setg(Base::pbase(), Base::pbase(), Base::pbase() + avail);
            Base::setp(Base::pbase(), Base::epptr());
            Base::pbump(avail);

            return true;
        }

        bool move(std::size_t size) {
            if (size > (Base::epptr() - Base::pptr())) {
                return false;
            }

            Base::pbump(size);

            return false;
        }
    };

    typedef ByteBuf<> ByteBuffer;

    template<std::size_t SIZE, class CharT = char>
    class ByteBufFix : public ByteBuf<CharT> {
        std::array<CharT, SIZE> _buf{};
    public:
        ByteBufFix() {
            ByteBuf<CharT>::setp(_buf.begin(), _buf.end());
            ByteBuf<CharT>::setg(_buf.begin(), _buf.begin(), _buf.begin());
        }
    };

    template<typename T>
    class ByteBufferRef {
        const T *_data;
        size_t _size;
    public:
        ByteBufferRef(std::vector<T> &data, size_t size)
                : _data(data.data()), _size(std::min(size, data.size())) {}

        ByteBufferRef(T *data, size_t size)
                : _data(data), _size(size) {}

        ByteBufferRef(ByteBuffer &bb)
                : _data((const T *) bb.data()), _size(bb.size()) {}

        const T *data() const {
            return _data;
        }

        size_t size() const {
            return _size;
        }
    };

}
