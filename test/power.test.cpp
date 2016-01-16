
#include <sdl++/power.hpp>

#include "SDL.h"

#include <gtest/gtest.h>

using namespace std::chrono_literals;

struct power : ::testing::Test {
    power() { ::SDL_Init(SDL_INIT_EVENTS); }

    ~power() { ::SDL_Quit(); }
};

TEST_F(power, rel_ops) {
    sdl::power_info p1{};
    sdl::power_info p2{sdl::power_state::on_battery, 3600s, 58};

    EXPECT_EQ(p1, p1);
    EXPECT_EQ(p2, p2);
    EXPECT_NE(p1, p2);
}

TEST_F(power, get_info) {
    auto info = sdl::get_power_info();

    int c_secs, c_pct;
    auto c_state = SDL_GetPowerInfo(&c_secs, &c_pct);

    EXPECT_EQ(c_state, static_cast<SDL_PowerState>(info.state));
    EXPECT_EQ(c_secs, info.secs_left.value_or(-1s).count());
    EXPECT_EQ(c_pct, info.percent_left.value_or(-1));
}