
#include <sdl++/power.hpp>

#include "SDL.h"

#include <gtest/gtest.h>

struct power : ::testing::Test {
    power() { ::SDL_Init(SDL_INIT_EVENTS); }

    ~power() { ::SDL_Quit(); }
};

TEST_F(power, basic) {
    auto info = sdl::get_power_info();

    int c_secs, c_pct;
    auto c_state = SDL_GetPowerInfo(&c_secs, &c_pct);

    EXPECT_EQ(c_state, static_cast<SDL_PowerState>(info.state));
    EXPECT_EQ(c_secs, info.secs_left.value_or(-1));
    EXPECT_EQ(c_pct, info.percent_left.value_or(-1));
}