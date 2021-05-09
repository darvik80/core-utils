#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#undef main

#include "audioplayer/AoAudioPlayer.h"
#include <functional>

#include <fftw3.h>
#include <math.h>
#include <sys/semaphore.h>
#include <pthread.h>
#include <thread>
#include <fmt/format.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//Key press surfaces constants
enum KeyPressSurfaces {
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

struct HSV {
    double h, s, v;
};

struct RGB {
    double r, g, b;
};

void createRGB(struct RGB *color, double r, double g, double b) {
    color->r = r;
    color->g = g;
    color->b = b;
}

void conversion(struct HSV in, struct RGB *out) {
    double c, m, x;
    c = m = x = 0.0;

    c = in.v * in.s;
    x = c * (1.0 - fabs(fmod(in.h / 60, 2) - 1.0));
    m = in.v - c;

    if (in.h >= 0.0 && in.h < 60.0)
        createRGB(out, c + m, x + m, m);
    else if (in.h >= 60.0 && in.h < 120.0)
        createRGB(out, x + m, c + m, m);
    else if (in.h >= 120.0 && in.h < 180.0)
        createRGB(out, m, c + m, x + m);
    else if (in.h >= 180.0 && in.h < 240.0)
        createRGB(out, m, x + m, c + m);
    else if (in.h >= 240.0 && in.h < 300.0)
        createRGB(out, x + m, m, c + m);
    else if (in.h >= 300 && in.h < 360.0)
        createRGB(out, c + m, m, x + m);
    else
        createRGB(out, m, m, m);

    out->r *= 255;
    out->g *= 255;
    out->b *= 255;
}

struct AudioData {
    Uint8 *position;
    Uint32 length;
    SDL_AudioFormat format;
    fftw_plan plan;
    fftw_complex *in;
    fftw_complex *out;
    SDL_Renderer *renderer;
    struct RGB *color;
    SDL_Point *time_domain;
};

struct wrapper {
    Uint8 *stream;
    struct AudioData *audio;
};

static sem_t play;
//no MUTEX on this int because only data.c->visualizerOutput uses it;
static int NSAMPLES{2048};
static int MODE = 1;

double Get16bitAudioSample(Uint8 *, SDL_AudioFormat);

void *visualizerOutput(void *);

void changeMode();

#define WIDTH 800
#define HEIGHT 600
#define SAMPLE_RATE 44100
#define BARS 60
#define THICKNESS 15
#define DISTANCE 16
#define FIT_FACTOR 30

std::mutex mutex;
std::vector<uint8_t> sample;

void myCallback(void *userData, Uint8 *stream, int len) {
    AudioData *audio = (struct AudioData *) userData;
    wrapper wrap;

    if (len == 0)
        return;

    mutex.lock();
    sample.resize(len);
    SDL_memcpy(sample.data(), stream, len);
    mutex.unlock();

}

double Get16bitAudioSample(Uint8 *bytebuffer, SDL_AudioFormat format) {
    Uint16 val = 0x0;

    if (SDL_AUDIO_ISLITTLEENDIAN(format))
        val = (uint16_t) bytebuffer[0] | ((uint16_t) bytebuffer[1] << 8);
    else
        val = ((uint16_t) bytebuffer[0] << 8) | (uint16_t) bytebuffer[1];

    if (SDL_AUDIO_ISSIGNED(format))
        return ((int16_t) val) / 32768.0;

    return val / 65535.0;
}

void *visualizerOutput(void *arg) {
    auto *wrap = (struct wrapper *) arg;
    double max[BARS];
    double multiplier;
    int window_size = 2;
    int count = 0;
    int sum;
    //double freq_bin[BARS+1] = {19.0,140.0,250.0,400.0,500.0,600.0,700.0,800.0,1000.0,1500.0,(double)SAMPLE_RATE/2};
    double freq_bin[BARS + 1];
    double re, im;
    float CONSTANT = (float) NSAMPLES / WIDTH;
    float freq;
    double magnitude;
    int startx = 0, starty = HEIGHT;
    struct HSV hsv;
    static int colorstart = 0;
    //construct a range of frequencies based on NSAMPLES
    for (int i = 0; i < BARS; i++) {
        max[i] = 1.7E-308;
        freq_bin[i] = i * (SAMPLE_RATE / NSAMPLES) + i;
    }
    freq_bin[BARS] = SAMPLE_RATE / 2;

    int len = 0;
    int cnt = NSAMPLES;
    for (int i = 0; i < cnt; i++) {
        //getting values from stream and applying hann windowing function
        multiplier = 0.5 * (1 - cos(2 * M_PI * i / cnt));

        auto val = Get16bitAudioSample(wrap->stream, wrap->audio->format) * multiplier;
        wrap->audio->in[i][0] = val;
        wrap->audio->in[i][1] = 0.0;

        wrap->stream += 2;
        len += 2;
    }

    //time domain visualizer (TESTING)
    if (MODE) {
        //
        //
        //INIT TIME DOMAIN MODE
        //
        //
        SDL_SetRenderDrawColor(wrap->audio->renderer, 0, 0, 0, 0);
        SDL_RenderClear(wrap->audio->renderer);
        hsv.h = (2 + colorstart) % 360;
        hsv.s = hsv.v = 1.0;
        conversion(hsv, wrap->audio->color);
        SDL_SetRenderDrawColor(wrap->audio->renderer,
                               wrap->audio->color->r,
                               wrap->audio->color->g,
                               wrap->audio->color->b,
                               255);
        for (int i = 0; i < NSAMPLES; i++) {
            wrap->audio->time_domain[i].x = i / CONSTANT;
            wrap->audio->time_domain[i].y = 300 - wrap->audio->in[i][0] * 160;
        }
        SDL_RenderDrawLines(wrap->audio->renderer, wrap->audio->time_domain, NSAMPLES);
    } else {
        fftw_execute(wrap->audio->plan);

        //calculate magnitudes
        for (int j = 0; j < NSAMPLES / 2; j++) {
            re = wrap->audio->out[j][0];
            im = wrap->audio->out[j][1];

            magnitude = sqrt((re * re) + (im * im));

            freq = j * ((double) SAMPLE_RATE / NSAMPLES);

            for (int i = 0; i < BARS; i++)
                if ((freq > freq_bin[i]) && (freq <= freq_bin[i + 1]))
                    if (magnitude > max[i])
                        max[i] = magnitude;
        }

        SDL_SetRenderDrawColor(wrap->audio->renderer, 0, 0, 0, 0);
        SDL_RenderClear(wrap->audio->renderer);

        //SDL_SetRenderDrawColor(audio->renderer,255,0,0,255);
        //effects techniques
        //max[i]=f(max[i])
        //
        //normal:       f(x)=FIT_FACTOR*x
        //exponential:  f(x)=log(x*FIT_FACTOR2)*FIT_FACTOR
        //multiPeak:    f(x)=x/Peak[i]*FIT_FACTOR
        //maxPeak:      f(x)=x/Global_Peak*FIT_FACTOR
        //TODO: substitute this whole system with SDL_DrawRects for performance
        //int SDL_RenderDrawRects(SDL_Renderer* renderer, const SDL_Rect* rects,int count)
        for (int i = 0; i < BARS; i++) {
            hsv.h = ((i * 2) + colorstart) % 360;
            hsv.s = hsv.v = 1.0;
            conversion(hsv, wrap->audio->color);
            SDL_SetRenderDrawColor(wrap->audio->renderer,
                                   wrap->audio->color->r,
                                   wrap->audio->color->g,
                                   wrap->audio->color->b,
                                   255);

            if (max[i] > 2.0)
                max[i] = log(max[i]);

            for (int j = 0; j < THICKNESS; j++)
                SDL_RenderDrawLine(wrap->audio->renderer,
                                   startx + (i * DISTANCE + j),
                                   starty-200,
                                   startx + (i * DISTANCE + j),
                                   starty - 200 - (FIT_FACTOR * max[i]));
        }
        //
        //
        //ENDING BARS MODE
        //
        //
    }
    colorstart += 2;
    SDL_RenderPresent(wrap->audio->renderer);
    //sem_post(&play);
}

class SDLApp {
private:
    SDL_Window *_window;
    SDL_Surface *_surface;
    SDL_Surface *_helloWorld;

    //The images that correspond to a keypress
    SDL_Surface *gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

    //Current displayed image
    SDL_Surface *gCurrentSurface = nullptr;


    //The window renderer
    //SDL_Renderer *_renderer = nullptr;


    Mix_Music *_music;

    AudioData *_audioData;
public:
    bool init() {
        //Initialization flag
        bool success = true;

        //Initialize SDL
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            success = false;
        } else {
            int result{0};
            if (!((result = Mix_Init(MIX_INIT_MP3)) && MIX_INIT_MP3)) {
                printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            }

            //Set texture filtering to linear
            if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
                printf("Warning: Linear texture filtering not enabled!");
            }

            //Create window
            _window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
            if (_window == nullptr) {
                printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
                success = false;
            } else {
                //Get window surface
                //_surface = SDL_GetWindowSurface(_window);


                //Create renderer for window
                //_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
                _audioData = new AudioData{};

                _audioData->renderer=SDL_CreateRenderer(_window,-1,SDL_RENDERER_ACCELERATED);
                if (_audioData->renderer == nullptr) {
                    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                    success = false;
                } else {
                    //Initialize renderer color
                    SDL_SetRenderDrawColor(_audioData->renderer, 0xFF, 0xFF, 0xFF, 0xFF);

                    //Initialize PNG loading
                    int imgFlags = IMG_INIT_PNG;
                    if (!(IMG_Init(imgFlags) & imgFlags)) {
                        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                        success = false;
                    }
                }
            }

            //Initialize SDL_mixer
            if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) < 0) {
                printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
                success = false;
            }

            _music = Mix_LoadMUS("resources/1577203012_max-brhon-the-future.mp3");
            if (_music == nullptr) {
                printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
                success = false;
            }
        }

        //memory allocation for the arrays
        _audioData->in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * NSAMPLES);
        _audioData->out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * NSAMPLES);
        _audioData->color = (struct RGB *) malloc(sizeof(RGB));
        _audioData->time_domain = (SDL_Point *) malloc(sizeof(SDL_Point) * NSAMPLES);

//        _audioData.length = 0x12000;
//        _audioData.position = new uint8_t[_audioData.length];
//        wav_spec.freq=SAMPLE_RATE;
//        wav_spec.channels=2;
//        wav_spec.samples=NSAMPLES;
//        wav_spec.callback=myCallback;
//        wav_spec.userdata=audio;
//        wav_spec.format=AUDIO_S16;
        _audioData->format = AUDIO_S16;



        //planning the forward discrete fourier transformation in 1 dimension
        //FFTW_FORWARD is a constant that select the forward method and
        //FFTW_ESTIMATE is an estimate of the time it should take(to verify)
        _audioData->plan = fftw_plan_dft_1d(NSAMPLES,
                                           _audioData->in,
                                           _audioData->out,
                                           FFTW_FORWARD, FFTW_MEASURE);

        return success;
    }

    SDL_Surface *loadSurface(std::string path) {
        //Load image at specified path
        SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str());
        if (loadedSurface == nullptr) {
            printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        return loadedSurface;
    }

    bool loadMedia() {
        //Loading success flag
        bool success = true;

        //Load splash image
        _helloWorld = SDL_LoadBMP("resources/hello_world.bmp");
        if (_helloWorld == nullptr) {
            printf("Unable to load image %s! SDL Error: %s\n", "02_getting_an_image_on_the_screen/hello_world.bmp", SDL_GetError());
            success = false;
        }

        //Load default surface
        gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("resources/press.bmp");
        if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == nullptr) {
            printf("Failed to load default image!\n");
            success = false;
        }

        //Load up surface
        gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("resources/up.bmp");
        if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == nullptr) {
            printf("Failed to load up image!\n");
            success = false;
        }

        //Load down surface
        gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("resources/down.bmp");
        if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == nullptr) {
            printf("Failed to load down image!\n");
            success = false;
        }

        //Load left surface
        gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("resources/left.bmp");
        if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == nullptr) {
            printf("Failed to load left image!\n");
            success = false;
        }

        //Load right surface
        gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("resources/right.bmp");
        if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == nullptr) {
            printf("Failed to load right image!\n");
            success = false;
        }

        return success;
    }

    void render() {
        mutex.lock();
        wrapper wrap{};

        wrap.stream = sample.data();
        wrap.audio = this->_audioData;
        visualizerOutput(&wrap);
        mutex.unlock();

    }
    bool update() {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            //User requests quit
            if (e.type == SDL_QUIT) {
                return true;
            } else if (e.type == SDL_KEYUP) {
                MODE = !MODE;
            }
        }

        return false;
    }

    int run() {
        //timing variables
        //Load media
        if (!loadMedia()) {
            printf("Failed to load media!\n");
        } else {
            Mix_SetPostMix(myCallback, _audioData);

            Mix_PlayMusic(_music, 0);

            //Main loop flag
            bool quit = false;

            //Event handler
            SDL_Event e;

            float UPDATE_RATE = 60.0;
            float UPDATE_INTERVAL = 1000/UPDATE_RATE;
            long IDLE_TIME = 1000000000;

            //While application is running
            int fps = 0, upd = 0, updl = 0;

            float delta = 0.0;
            long count = 0;
            long lastTime = SDL_GetTicks();
            while (!quit) {
                //time in milliseconds since the program started
                long now = SDL_GetTicks();
                long elapsedTime = now - lastTime;
                count += elapsedTime;

                lastTime = now;

                bool  render = false;
                delta += (float)elapsedTime / UPDATE_INTERVAL;
                while (delta > 1) {
                    quit = update();
                    delta--;
                    if (render) {
                        updl++;
                    } else {
                        render = true;
                    }
                    upd++;
                }

                if (render) {
                    this->render();
                    fps++;
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds {1});
                }

                if (count >= 1000) {
                    SDL_SetWindowTitle(_window, fmt::format("fps: {}, upd: {}, updl: {}", fps, upd, updl).c_str());
                    fps = 0; upd = 0; updl = 0;
                    count -= 1000;
                }
            }
        }

        return 0;
    }

    void close() {
        //Deallocate surface
        SDL_FreeSurface(_surface);
        _surface = nullptr;

        //Destroy window
        SDL_DestroyWindow(_window);
        _window = nullptr;

        Mix_CloseAudio();
        //Quit SDL subsystems
        IMG_Quit();
        Mix_Quit();
        SDL_Quit();
    }
};

int main(int argc, char *argv[]) {
    SDLApp app{};
    if (!app.init()) {
        return 1;
    }

    app.run();

    app.close();

    return 0;
}
