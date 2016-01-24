
#include <sdl++/video.hpp>

#include "SDL.h"

#include "catch.hpp"

namespace {

template <typename Container>
bool contains(const Container& container,
              const typename Container::value_type& value) {
    return std::find(std::begin(container), std::end(container), value) !=
           std::end(container);
}

void check_dm(const sdl::display_mode& lhs, const SDL_DisplayMode& rhs) {
    REQUIRE(lhs.format == rhs.format);
    REQUIRE(lhs.width == rhs.w);
    REQUIRE(lhs.height == rhs.h);
    REQUIRE(lhs.refresh_rate == rhs.refresh_rate);
    REQUIRE(lhs.driver_data == rhs.driverdata);
}
}

TEST_CASE("Can query video drivers", "[video]") {
    auto drivers = sdl::get_video_drivers();
    REQUIRE(drivers.size() > 0);
}

TEST_CASE("Can initialize video", "[video]") {
    // SDL will actually refuse to intialize the "dummy" driver unless the
    // environment variable SDL_VIDEODRIVER=dummy is set, so here we just
    // init the default one, for what that's worth.
    // Note that we can't even check SDL_WasInit(), as SDL_VideoInit() doesn't
    // set that bit
    sdl::video_init_guard{};
}

TEST_CASE("Can query displays", "[video]") {
    sdl::video_init_guard guard{};

    auto displays = sdl::get_displays();
    REQUIRE(displays.size() == SDL_GetNumVideoDisplays());

    int index = 0;
    for (const auto& d : displays) {
        SECTION("sdl::display::operator bool()") { REQUIRE(d); }

        SECTION("sdl::display::get_name()") {
            REQUIRE(d.get_name() != nullptr);
            REQUIRE(std::string(d.get_name()) == SDL_GetDisplayName(index));
        }

        SECTION("sdl::display::all_modes()") {
            auto all_modes = d.get_modes();
            REQUIRE(all_modes.size() == SDL_GetNumDisplayModes(index));
        }

        SECTION("sdl::display::current_mode()") {
            SDL_DisplayMode cm;
            SDL_GetCurrentDisplayMode(index, &cm);
            check_dm(d.get_current_mode(), cm);
        }

        SECTION("sdl::display::desktop_mode()") {
            SDL_DisplayMode dm;
            SDL_GetDesktopDisplayMode(index, &dm);
            check_dm(d.get_desktop_mode(), dm);
        }

#if 0 // This test always seems to throw, no matter what
        SECTION("sdl::display::get_dpi()") {
            float ddpi = 0, hdpi = 0, vdpi = 0;
            SDL_GetDisplayDPI(index, &ddpi, &hdpi, &vdpi);
            REQUIRE(d.get_dpi() == std::tie(ddpi, hdpi, vdpi));
        }
#endif

        SECTION("sdl::display::get_bounds()") {
            auto r1 = d.get_bounds();
            SDL_Rect r2;
            SDL_GetDisplayBounds(index, &r2);
            REQUIRE(SDL_RectEquals(&r1, &r2) == SDL_TRUE);
        }

        SECTION("sdl::display::get_closest_mode()") {
            const auto current = d.get_current_mode();
            auto closest = d.get_closest_mode(current);
            REQUIRE(closest == current);

            // I can't seem to get SDL to *not* return a closest mode, no
            // matter what nonsense I ask it for...
        }

        index++;
    }
}

TEST_CASE("sdl::window", "[video][window]") {}
