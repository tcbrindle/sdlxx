
#include "SDL.h"

#include <sdl++/init.hpp>
#include <sdl++/log.hpp>
#include <sdl++/timer.hpp>

using namespace std::chrono_literals;

int main(int, char**) {
    auto init = sdl::init_guard{sdl::init_flags::video};

    sdl::log("Creating window");
    auto window = SDL_CreateWindow("sdl++ Example", SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);

    sdl::log("Creating renderer");
    auto renderer = SDL_CreateRenderer(
        window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    sdl::delay(2s);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
