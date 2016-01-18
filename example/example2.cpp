
#include "SDL.h"

#include <sdl++/init.hpp>
#include <sdl++/events.hpp>

int main(int, char**) {
    auto init = sdl::init_guard{sdl::init_flags::everything};

    auto window =
        SDL_CreateWindow("Press escape to close", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);

    auto renderer = SDL_CreateRenderer(
        window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    bool quit = false;
    while (!quit) {
        sdl::optional<sdl::event> e = sdl::nullopt;
        while ((e = sdl::event_queue::poll())) {
            sdl::apply(e.value(),
                       [&quit](SDL_KeyboardEvent ke) {
                           if (ke.type == SDL_KEYDOWN &&
                               ke.keysym.sym == SDLK_ESCAPE) {
                               quit = true;
                           }
                       },
                       [&quit](SDL_QuitEvent) { quit = true; }, [](auto) {});
        }

        // Draw a frame
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
