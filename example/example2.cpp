
#include "SDL.h"

#include <sdl++/init.hpp>
#include <sdl++/video.hpp>

int main(int, char**) {
    sdl::init_guard init{};

    auto window =
        sdl::window("Press escape to close", sdl::windowpos::undefined,
                    sdl::windowpos::undefined, 800, 600);

    auto renderer =
        SDL_CreateRenderer(unwrap(window), 0, SDL_RENDERER_ACCELERATED |
                                                  SDL_RENDERER_PRESENTVSYNC);

    bool quit = false;
    while (!quit) {
        SDL_Event event;

        // Enter the event loop
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) { quit = true; }
                break;
            }
        }

        // Draw a frame
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);

    return 0;
}
