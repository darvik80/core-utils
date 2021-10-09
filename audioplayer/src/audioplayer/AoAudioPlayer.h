//
// Created by Ivan Kishchenko on 08.05.2021.
//

#pragma once

#include <ao/ao.h>
#include <thread>

#include "Mpg123Decoder.h"

class AoAudioPlayerLibrary {
public:
    AoAudioPlayerLibrary();

    ~AoAudioPlayerLibrary();
};

class AudioPlayer : public Mpg123DecoderCallback {
    std::thread _thread;
    ao_device *_dev{nullptr};
    ao_sample_format _format{};

    Mpg123Decoder _decoder;

    std::atomic<bool> _stop{};
public:
    void onFormatInfo(long rate, int channels, int encoding) override;

    void onData(const uint8_t *data, size_t size) override;

    void play(std::string_view filePath);

    void stop();

    void wait();

private:
    void run(std::string_view filePath);
};

