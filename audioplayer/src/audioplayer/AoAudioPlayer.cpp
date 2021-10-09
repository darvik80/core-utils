//
// Created by Ivan Kishchenko on 08.05.2021.
//

#include "AoAudioPlayer.h"

AoAudioPlayerLibrary::AoAudioPlayerLibrary() {
    ao_initialize();
}

AoAudioPlayerLibrary::~AoAudioPlayerLibrary() {
    ao_shutdown();
}

void AudioPlayer::onFormatInfo(long rate, int channels, int encoding) {
    int driver = ao_default_driver_id();
    _format.bits = mpg123_encsize(encoding) * 8;
    _format.rate = rate;
    _format.channels = channels;
    _format.byte_format = AO_FMT_NATIVE;
    _format.matrix = 0;
    _dev = ao_open_live(driver, &_format, NULL);
}

void AudioPlayer::onData(const uint8_t *data, size_t size) {
    ao_play(_dev, (char*)data, size);
}

void AudioPlayer::play(std::string_view filePath) {
    _stop = false;
    _thread = std::thread{[this, filePath]() { run(filePath.data()); }};
}

void AudioPlayer::stop() {
    _stop = true;
    if (_thread.joinable()) {
        _thread.join();
    }
}

void AudioPlayer::wait() {
    _thread.join();
}

void AudioPlayer::run(std::string_view filePath) {
    _decoder.open(filePath, this);
    while (!_stop) {
        int res = _decoder.process();
        if (res == MPG123_DONE) {
            _stop = true;
        }
    }
    _decoder.close();
}
