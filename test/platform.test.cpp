
#include <sdl++/platform.hpp>

#include <gtest/gtest.h>

TEST(platform, get_name) {
    EXPECT_STREQ(SDL_GetPlatform(), sdl::get_platform_name());
}