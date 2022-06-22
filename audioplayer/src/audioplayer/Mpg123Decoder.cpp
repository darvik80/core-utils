//
// Created by Ivan Kishchenko on 08.05.2021.
//

#include "Mpg123Decoder.h"

Mpg123DecoderLibrary::Mpg123DecoderLibrary() {
    mpg123_init();
}

Mpg123DecoderLibrary::~Mpg123DecoderLibrary() {
    mpg123_exit();
}

int Mpg123Decoder::open(std::string_view filePath, Mpg123DecoderCallback *callback) {
    int err;
    auto handler = mpg123_new(nullptr, &err);
    if (err != MPG123_OK) {
        return err;
    }
    auto size = mpg123_outblock(handler);
    _buffer.resize(size / 2);
    _callback = callback;

    err = mpg123_open(handler, filePath.data());
    if (err != MPG123_OK) {
        mpg123_delete(handler);
        return err;
    }

    _handler = handler;

    if (_callback) {
        int channels, encoding;
        long rate;
        mpg123_getformat(handler, &rate, &channels, &encoding);
        _callback.value()->onFormatInfo(rate, channels, encoding);
    }

    return MPG123_OK;
}

int Mpg123Decoder::process() {
    size_t size = 0;
    int res = mpg123_read(_handler, _buffer.data(), _buffer.size() * 2, &size);
    if (res == MPG123_OK && _callback) {
        for (int idx = 0; idx < size; idx += 2) {
            //_buffer[idx] = _buffer[idx]/2;
            //_buffer[idx+1] = 0;
        }
        _callback.value()->onData((const uint8_t *) _buffer.data(), size);

        //std::string str = fmt::sprintf("%.06d ", (*(uint16_t*)(_buffer.data())));
        //str += fmt::sprintf("%.06d ", (*(uint16_t*)(_buffer.data() + 2)));

        //logging::info("AMP: {}", str);
    }

    return res;
}

int Mpg123Decoder::close() {
    if (_handler) {
        auto err = mpg123_close(_handler);
        if (err != MPG123_OK) {
            return err;
        }
    }

    mpg123_delete(_handler);
    _handler = nullptr;
    return MPG123_OK;
}
