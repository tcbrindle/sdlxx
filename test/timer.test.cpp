
#include <sdl++/timer.hpp>

#include "SDL.h"

#include <gtest/gtest.h>

using namespace std::chrono_literals;

struct clock : ::testing::Test {
    clock() { SDL_Init(SDL_INIT_TIMER); }

    ~clock() { SDL_Quit(); }
};

TEST_F(clock, basic) {
    auto p1 = sdl::clock::now();

    auto p2 = p1 + 2s;

    EXPECT_EQ(2000, (p2 - p1).count());
}

TEST_F(clock, timer) {
    int call_count = 0;
    auto t = sdl::make_timeout(10ms, [&call_count](auto) -> sdl::duration {
        call_count++;
        return 10ms;
    });

    sdl::delay(25ms);

    EXPECT_EQ(2, call_count);
}
