
#include "SDL.h"

#include <sdl++/events.hpp>
#include <sdl++/init.hpp>

int main(int, char**) {
    auto init = sdl::init_guard{sdl::init_flags::everything};

    auto* window =
        SDL_CreateWindow("Press escape to close", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);

    auto renderer = SDL_CreateRenderer(
        window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    bool quit = false;
    while (!quit) {
        sdl::optional<sdl::event> e{};
        while ((e = sdl::poll_event())) {

            const auto visitor = sdl::make_lambda_visitor(
                [&quit](const sdl::keydown_event& ke) {
                    if (ke.state == sdl::button_state::pressed &&
                        ke.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                },
                [&quit](const sdl::quit_event&) { quit = true; },
                [window](const sdl::drop_event& d) {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                             "File dropped on window",
                                             d.file.c_str(), window);
                },
                [](const auto&) {});

            visit(visitor, e.value());
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
