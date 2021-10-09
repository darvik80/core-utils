//
// Created by Ivan Kishchenko on 08.05.2021.
//

#pragma once

#include <mpg123.h>
#include <optional>
#include <vector>
#include <string>

class Mpg123DecoderLibrary {
public:
    Mpg123DecoderLibrary();

    ~Mpg123DecoderLibrary();
};

class Mpg123DecoderCallback {
public:
    virtual void onFormatInfo(long rate, int channels, int encoding) = 0;

    virtual void onData(const uint8_t *data, size_t size) = 0;
};

class Mpg123Decoder {
private:
    mpg123_handle *_handler;
    std::vector<int16_t> _buffer;
    std::optional<Mpg123DecoderCallback *> _callback;
public:
    int open(std::string_view filePath, Mpg123DecoderCallback *callback);

    int process();

    int close();
};
