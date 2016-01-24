
#include "SDL.h"

#include <sdl++/init.hpp>
#include <sdl++/log.hpp>
#include <sdl++/timer.hpp>
#include <sdl++/video.hpp>

using namespace std::chrono_literals;

int main(int, char**) {
    sdl::init_guard init{};

    sdl::log() << "Creating window";
    auto window = sdl::window("sdl++ Example", sdl::windowpos::undefined,
                              sdl::windowpos::undefined, 800, 600,
                              sdl::window_flags::resizable);

    window.set_fullscreen(sdl::fullscreen_mode::fullscreen_desktop);

    sdl::log() << "Creating renderer";
    auto renderer = SDL_CreateRenderer(
        unwrap(window), 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    sdl::delay(2s);

    SDL_DestroyRenderer(renderer);

    return 0;
}
