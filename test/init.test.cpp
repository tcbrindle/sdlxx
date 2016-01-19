
#include <sdl++/init.hpp>

#include <gtest/gtest.h>

namespace {

inline void expect_initted(uint32_t flags) {
    EXPECT_NE(SDL_FALSE, SDL_WasInit(flags));
}

inline void expect_not_initted(uint32_t flags = SDL_INIT_EVERYTHING) {
    EXPECT_EQ(SDL_FALSE, SDL_WasInit(flags));
}
}

TEST(init, init_flags) {
    sdl::init_flags f = sdl::init_flags::timer | sdl::init_flags::audio;

    EXPECT_TRUE(sdl::flag_is_set(f, sdl::init_flags::audio));
    EXPECT_FALSE(sdl::flag_is_set(f, sdl::init_flags::video));
}

TEST(init, init_guard_flags) {
    expect_not_initted();

    {
        sdl::init_guard guard{sdl::init_flags::events | sdl::init_flags::timer};

        expect_initted(SDL_INIT_EVENTS | SDL_INIT_TIMER);
    }

    expect_not_initted();
}

TEST(init, init_guard_ilist) {
    expect_not_initted();

    {
        sdl::init_guard guard{sdl::init_flags::events, sdl::init_flags::timer};

        expect_initted(SDL_INIT_EVENTS | SDL_INIT_TIMER);
    }

    expect_not_initted();
}

TEST(init, subsystem_guard_flags) {
    expect_not_initted();

    {
        sdl::subsystem_init_guard guard{sdl::init_flags::events};

        expect_initted(SDL_INIT_EVENTS);
    }

    expect_not_initted();
}

TEST(init, subsystem_guard_copy) {
    expect_not_initted();

    auto p1 = new sdl::subsystem_init_guard(sdl::init_flags::events);

    expect_initted(SDL_INIT_EVENTS);

    {
        auto p2 = *p1;
        expect_initted(SDL_INIT_EVENTS);

        delete p1;

        expect_initted(SDL_INIT_EVENTS);
    }

    expect_not_initted();
}

TEST(init, subsystem_guard_assign) {
    expect_not_initted();

    {
        sdl::subsystem_init_guard g1{sdl::init_flags::events};
        expect_initted(SDL_INIT_EVENTS);

        {
            sdl::subsystem_init_guard g2{sdl::init_flags::timer};
            expect_initted(SDL_INIT_TIMER);

            g1 = g2;
        }

        expect_not_initted(SDL_INIT_EVENTS);
        expect_initted(SDL_INIT_TIMER);
    }

    expect_not_initted();
}
