
#include <sdl++/events.hpp>
#include <sdl++/log.hpp>

#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

struct timer {
    using time_point = std::chrono::high_resolution_clock::time_point;

    std::chrono::milliseconds elapsed() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - _start);
    }

    time_point _start = std::chrono::high_resolution_clock::now();
};

int main(int, char**) {
    constexpr auto timeout = 3s;
    auto c_count = 0;
    auto cpp_switch_count = 0;
    auto cpp_match_count = 0;

    {
        auto t = timer{};
        while (t.elapsed() < timeout) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                case SDL_USEREVENT:
                    c_count++;
                }
            }
            SDL_Event push_event;
            push_event.type = SDL_USEREVENT;
            SDL_PushEvent(&push_event);
        }
    }

    {
        auto t = timer{};
        while (t.elapsed() < timeout) {
            sdl::optional<sdl::event> e;
            while ((e = sdl::poll_event())) {
                switch (e->index()) {
                case sdl::event_type::user:
                    cpp_switch_count++;
                }
            }
            SDL_Event push_event;
            push_event.type = SDL_USEREVENT;
            SDL_PushEvent(&push_event);
        }
    }

    {
        auto t = timer{};
        while (t.elapsed() < timeout) {
            sdl::optional<sdl::event> e;
            while ((e = sdl::poll_event())) {
                sdl::match(*e,
                           [&cpp_match_count](const sdl::user_event&) {
                               cpp_match_count++;
                           },
                           [](const auto&) {});
            }
            SDL_Event push_event;
            push_event.type = SDL_USEREVENT;
            SDL_PushEvent(&push_event);
        }
    }

    std::cout << "SDL_Event loop: " << c_count / timeout.count()
              << " events per second\n";
    std::cout << "sdl::event switch loop: "
              << cpp_switch_count / timeout.count()
              << " events per second (overhead "
              << 100.0 * (c_count - cpp_switch_count) / double(c_count)
              << "%)\n";
    std::cout << "sdl::event visitor loop: "
              << cpp_match_count / timeout.count()
              << " events per second (overhead "
              << 100.0 * (c_count - cpp_match_count) / double(c_count)
              << "%)\n";

    return 0;
}