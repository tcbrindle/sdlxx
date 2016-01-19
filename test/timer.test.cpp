
#include <sdl++/timer.hpp>

#include "SDL.h"

#include <gtest/gtest.h>

using namespace std::chrono_literals;

struct timer : ::testing::Test {
    timer() { SDL_Init(SDL_INIT_TIMER); }

    ~timer() { SDL_Quit(); }
};

TEST_F(timer, clock) {
    auto t1 = SDL_GetTicks();
    auto t2 = sdl::clock::now();

    EXPECT_LE(t1, t2.time_since_epoch().count());
    EXPECT_LE(sdl::duration(t1), t2.time_since_epoch());
}

TEST_F(timer, hiperf_counter) {
    auto c1 = SDL_GetPerformanceCounter();
    auto c2 = sdl::get_performance_counter();

    EXPECT_LE(c1, c2);
}

TEST_F(timer, hiperf_freq) {
    EXPECT_EQ(SDL_GetPerformanceFrequency(), sdl::get_performance_frequency());
}

TEST_F(timer, delay) {
    auto t1 = SDL_GetTicks();

    // Delay for 25 microseconds
    sdl::delay(25ms);

    auto t2 = SDL_GetTicks();

    // t2 should be in the region of 25ms after t1
    // This may not be exact, so give a small margin of error
    EXPECT_NEAR(t1 + 25, t2, 5);
}

TEST_F(timer, timeout) {
    int call_count = 0;
    {
        auto t = sdl::make_timeout(10ms, [&call_count](auto) {
            call_count++;
            return 10ms;
        });

        // Allow the callback to be called twice, with a little leeway
        sdl::delay(25ms);
    }

    EXPECT_EQ(2, call_count);
}
