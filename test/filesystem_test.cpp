
#include <sdl++/filesystem.hpp>

#include "catch.hpp"

TEST_CASE("sdl::get_base_path() is wrapped correctly", "[filesystem]") {
    auto c_str = SDL_GetBasePath();
    auto cpp_str = sdl::get_base_path();

    if (!c_str) {
        REQUIRE_FALSE(cpp_str);
    } else {
        REQUIRE(c_str == cpp_str.value());
    }
}

TEST_CASE("sdl::get_pref_path() is wrapped correctly", "[filesystem]") {
    constexpr const char* org = "com.github.tcbrindle";
    constexpr const char* app = "sdl++ Test";

    auto c_str = SDL_GetPrefPath(org, app);
    auto cpp_str = sdl::get_pref_path(org, app);

    if (!c_str) {
        REQUIRE_FALSE(cpp_str);
    } else {
        REQUIRE(c_str == cpp_str.value());
    }
}
