
#include <sdl++/filesystem.hpp>

#include "catch.hpp"

TEST_CASE("sdl::get_base_path() is wrapped correctly", "[filesystem]") {
    char* c_path = SDL_GetBasePath();

    if (c_path) {
        REQUIRE(sdl::get_base_path() == c_path);
    } else {
        REQUIRE_THROWS_AS(sdl::get_base_path(), sdl::error);
    }

    SDL_free(c_path);
}

TEST_CASE("sdl::get_pref_path() is wrapped correctly", "[filesystem]") {
    constexpr const char* org = "com.github.tcbrindle";
    constexpr const char* app = "sdl++ Test";

    char* c_path = SDL_GetPrefPath(org, app);

    if (c_path) {
        REQUIRE(sdl::get_pref_path(org, app) == c_path);
    } else {
        REQUIRE_THROWS_AS(sdl::get_pref_path(org, app), sdl::error);
    }

    SDL_free(c_path);
}
