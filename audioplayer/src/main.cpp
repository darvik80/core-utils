//#include "audioplayer/AudioPlayer.h"

#include <mpg123.h>
#include <ao/ao.h>

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <thread>
#include <logging/Logger.h>
#include <fmt/printf.h>

class Mpg123DecoderLibrary {
public:
    Mpg123DecoderLibrary() {
        mpg123_init();
    }

    ~Mpg123DecoderLibrary() {
        mpg123_exit();
    }
};

class Mpg123DecoderCallback {
public:
    virtual void onFormatInfo(long rate, int channels, int encoding) = 0;

    virtual void onData(const uint8_t *data, size_t size) = 0;
};

class Mpg123Decoder {
private:
    mpg123_handle *_handler;
    std::vector<uint16_t> _buffer;
    std::optional<Mpg123DecoderCallback *> _callback;
public:
    int open(std::string_view filePath, Mpg123DecoderCallback *callback) {
        int err;
        auto handler = mpg123_new(nullptr, &err);
        if (err != MPG123_OK) {
            return err;
        }
        auto size = mpg123_outblock(handler);
        _buffer.resize(size/2);
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

    int process() {
        size_t size = 0;
        int res = mpg123_read(_handler, _buffer.data(), _buffer.size()*2, &size);
        if (res == MPG123_OK && _callback) {
            for (int idx = 0; idx < size; idx += 2) {
                //_buffer[idx] = _buffer[idx]/2;
                //_buffer[idx+1] = 0;
            }
            _callback.value()->onData((const uint8_t *)_buffer.data(), size);

            //std::string str = fmt::sprintf("%.06d ", (*(uint16_t*)(_buffer.data())));
            //str += fmt::sprintf("%.06d ", (*(uint16_t*)(_buffer.data() + 2)));

            //logging::info("AMP: {}", str);
        }

        return res;
    }

    int close() {
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
};

class AudioPlayerLibrary {
public:
    AudioPlayerLibrary() {
        ao_initialize();
    }

    ~AudioPlayerLibrary() {
        ao_shutdown();
    }
};

class AudioPlayer : public Mpg123DecoderCallback {
    std::thread _thread;
    ao_device *_dev{nullptr};
    ao_sample_format _format{};

    Mpg123Decoder _decoder;

    std::atomic<bool> _stop{};
public:
    void onFormatInfo(long rate, int channels, int encoding) override {
        int driver = ao_default_driver_id();
        _format.bits = mpg123_encsize(encoding) * 8;
        _format.rate = rate;
        _format.channels = channels;
        _format.byte_format = AO_FMT_NATIVE;
        _format.matrix = 0;
        _dev = ao_open_live(driver, &_format, NULL);
    }

    void onData(const uint8_t *data, size_t size) override {
        ao_play(_dev, (char*)data, size);
    }

    void play(std::string_view filePath) {
        _stop = false;
        _thread = std::thread{[this, filePath]() { run(filePath.data()); }};
    }

    void stop() {
        _stop = true;
        if (_thread.joinable()) {
            _thread.join();
        }
    }

    void wait() {
        _thread.join();
    }

private:
    void run(std::string filePath) {
        _decoder.open(filePath, this);
        while (!_stop) {
            int res = _decoder.process();
            if (res == MPG123_DONE) {
                _stop = true;
            }
        }
        _decoder.close();
    }
};

int main(int argc, char *argv[]) {
    Mpg123DecoderLibrary decoderLibrary;
    AudioPlayerLibrary audioPlayerLibrary;

    AudioPlayer player;
    player.play(argv[1]);
    player.wait();
    return 0;
}
