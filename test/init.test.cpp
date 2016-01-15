
#include <sdl++/init.hpp>

#include <gtest/gtest.h>

TEST(init, init_flags) {
    sdl::init_flags f = sdl::init_flags::timer | sdl::init_flags::audio;

    EXPECT_TRUE(sdl::flag_is_set(f, sdl::init_flags::audio));
    EXPECT_FALSE(sdl::flag_is_set(f, sdl::init_flags::video));
}

TEST(init, init_guard_flags) {
    EXPECT_EQ(SDL_FALSE, SDL_WasInit(SDL_INIT_EVERYTHING));

    {
        sdl::init_guard guard{sdl::init_flags::events | sdl::init_flags::timer};

        EXPECT_NE(SDL_FALSE, SDL_WasInit(SDL_INIT_EVENTS));
        EXPECT_NE(SDL_FALSE, SDL_WasInit(SDL_INIT_TIMER));
    }

    EXPECT_EQ(SDL_FALSE, SDL_WasInit(SDL_INIT_EVENTS | SDL_INIT_TIMER));
}

TEST(init, init_guard_ilist) {
    EXPECT_EQ(SDL_FALSE, SDL_WasInit(SDL_INIT_EVERYTHING));

    {
        sdl::init_guard guard{sdl::init_flags::events, sdl::init_flags::timer};

        EXPECT_NE(SDL_FALSE, SDL_WasInit(SDL_INIT_EVENTS));
        EXPECT_NE(SDL_FALSE, SDL_WasInit(SDL_INIT_TIMER));
    }

    EXPECT_EQ(SDL_FALSE, SDL_WasInit(SDL_INIT_EVENTS | SDL_INIT_TIMER));
}
