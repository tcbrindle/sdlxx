
#include <SDL_video.h>
#include <sdl++/video.hpp>

#include "SDL.h"

#include "catch.hpp"

using namespace std::string_literals;

namespace {

void check_dm(const sdl::display_mode& lhs, const SDL_DisplayMode& rhs) {
    CHECK(lhs.format == rhs.format);
    CHECK(lhs.width == rhs.w);
    CHECK(lhs.height == rhs.h);
    CHECK(lhs.refresh_rate == rhs.refresh_rate);
    CHECK(lhs.driver_data == rhs.driverdata);
}
}

TEST_CASE("Can query video drivers", "[video]") {
    auto drivers = sdl::get_video_drivers();
    CHECK(drivers.size() > 0);
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
    CHECK(displays.size() == SDL_GetNumVideoDisplays());

    int index = 0;
    for (const auto& d : displays) {
        SECTION("sdl::display::operator bool()") { CHECK(d); }

        SECTION("sdl::display::get_name()") {
            CHECK(d.get_name() != nullptr);
            CHECK(std::string(d.get_name()) == SDL_GetDisplayName(index));
        }

        SECTION("sdl::display::all_modes()") {
            auto all_modes = d.get_modes();
            CHECK(all_modes.size() == SDL_GetNumDisplayModes(index));
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
            CHECK(d.get_dpi() == std::tie(ddpi, hdpi, vdpi));
        }
#endif

        SECTION("sdl::display::get_bounds()") {
            auto r1 = d.get_bounds();
            SDL_Rect r2;
            SDL_GetDisplayBounds(index, &r2);
            CHECK(SDL_RectEquals(&r1, &r2) == SDL_TRUE);
        }

        SECTION("sdl::display::get_closest_mode()") {
            const auto current = d.get_current_mode();
            auto closest = d.get_closest_mode(current);
            CHECK(closest == current);

            // I can't seem to get SDL to *not* return a closest mode, no
            // matter what nonsense I ask it for...
        }

        index++;
    }
}

static_assert(sizeof(sdl::window) == sizeof(SDL_Window*), "");
static_assert(sizeof(sdl::window_ref) == sizeof(SDL_Window*), "");

template <typename Window>
void test_window_read_api(const Window& window) {
    CHECK(window);

    auto* c_win = sdl::to_c_value(window);

    CHECK(c_win != nullptr);
    CHECK(SDL_GetWindowTitle(c_win) == "Title"s);
    CHECK(window.get_title() == "Title"s);
    CHECK(window.get_display().get_index() == SDL_GetWindowDisplayIndex(c_win));
    {
        SDL_DisplayMode mode;
        SDL_GetWindowDisplayMode(c_win, &mode);
        CHECK(window.get_display_mode() == sdl::from_c_value(mode));
    }
    CHECK(window.get_pixel_format() == SDL_GetWindowPixelFormat(c_win));
    CHECK(window.get_id() == SDL_GetWindowID(c_win));
    CHECK(static_cast<SDL_WindowFlags>(window.get_flags()) ==
          SDL_GetWindowFlags(c_win));
    {
        int x, y;
        SDL_GetWindowPosition(c_win, &x, &y);
        CHECK(window.get_position() == std::make_pair(x, y));
    }
    {
        int w, h;
        SDL_GetWindowSize(c_win, &w, &h);
        CHECK(window.get_size() == std::make_pair(w, h));
    }
    {
        int w, h;
        SDL_GetWindowMinimumSize(c_win, &w, &h);
        CHECK(window.get_minimum_size() == std::make_pair(w, h));
    }
    {
        int w, h;
        SDL_GetWindowMaximumSize(c_win, &w, &h);
        CHECK(window.get_maximum_size() == std::make_pair(w, h));
    }
    CHECK(window.get_brightness() == SDL_GetWindowBrightness(c_win));

    // Hmmm, this doesn't seem to work as advertised?
    SDL_ShowWindow(c_win);
    SDL_SetWindowGrab(c_win, SDL_TRUE);
    CHECK(window.get_grab() == SDL_GetWindowGrab(c_win));
    CHECK(sdl::get_grabbed_window() == SDL_GetGrabbedWindow());
    SDL_SetWindowGrab(c_win, SDL_FALSE);
}

template <typename Window>
void test_window_write_api(Window&& window) {
    CHECK(window);

    auto* c_win = sdl::to_c_value(window);
    CHECK(c_win != nullptr);

    {
        SDL_DisplayMode default_mode;
        SDL_GetWindowDisplayMode(c_win, &default_mode);

        sdl::display_mode in_mode{SDL_PIXELFORMAT_ARGB8888, 800, 600, 60};
        CHECK_NOTHROW(window.set_display_mode(in_mode));
        SDL_DisplayMode temp_mode;
        SDL_GetWindowDisplayMode(c_win, &temp_mode);
        CHECK(in_mode.format == temp_mode.format);
        CHECK(in_mode.width == temp_mode.w);
        CHECK(in_mode.height == temp_mode.h);

        CHECK_NOTHROW(window.set_default_display_mode());
        SDL_GetWindowDisplayMode(c_win, &temp_mode);
        CHECK(sdl::from_c_value(default_mode) == sdl::from_c_value(temp_mode));
    }

    window.set_title("New Title");
    CHECK(SDL_GetWindowTitle(c_win) == "New Title"s);

    // FIXME: Set icon

    {
        window.set_position(50, 50);
        int x, y;
        SDL_GetWindowPosition(c_win, &x, &y);
        CHECK(x == 50);
        CHECK(y == 50);
    }

    {
        window.set_size(400, 300);
        int w, h;
        SDL_GetWindowSize(c_win, &w, &h);
        CHECK(w == 400);
        CHECK(h == 300);
    }

    {
        window.set_minimum_size(120, 100);
        int w, h;
        SDL_GetWindowMinimumSize(c_win, &w, &h);
        CHECK(w == 120);
        CHECK(h == 100);
    }

    {
        window.set_maximum_size(1600, 1200);
        int w, h;
        SDL_GetWindowMaximumSize(c_win, &w, &h);
        CHECK(w == 1600);
        CHECK(h == 1200);
    }

    // We have no real way to test these are doing what they say, but we can
    // at least check the functions can be compiled and called without error
    CHECK_NOTHROW(window.set_bordered(true));
    CHECK_NOTHROW(window.show());
    CHECK_NOTHROW(window.hide());
    CHECK_NOTHROW(window.raise());
    CHECK_NOTHROW(window.minimize());
    CHECK_NOTHROW(window.maximize());
    CHECK_NOTHROW(window.restore());
    CHECK(window.set_fullscreen(sdl::fullscreen_mode::fullscreen));
    CHECK(window.set_fullscreen(sdl::fullscreen_mode::fullscreen_desktop));
    CHECK(window.set_fullscreen(sdl::fullscreen_mode::windowed));

    CHECK_NOTHROW(window.set_grab(true));
    // This doesn't seem to work with the dummy video driver
    if (SDL_GetCurrentVideoDriver() != "dummy"s) {
        CHECK(SDL_GetWindowGrab(c_win) == SDL_TRUE);
    }

    CHECK_NOTHROW(window.set_grab(false));
    CHECK(SDL_GetWindowGrab(c_win) == SDL_FALSE);

    // Likewise
    if (SDL_GetCurrentVideoDriver() != "dummy"s) {
        CHECK_NOTHROW(window.set_brightness(0.5f));
        CHECK(SDL_GetWindowBrightness(c_win) == 0.5f);
    }
}

TEST_CASE("sdl::window can be created", "[video][window]") {
    SECTION("...using full constructor") {
        auto window = sdl::window{
            "Title", sdl::windowpos::undefined, sdl::windowpos::undefined, 800,
            600,     sdl::window_flags::none};

        test_window_read_api(window);
        test_window_write_api(window);
    }
    SECTION("...using convenience constructor") {
        const auto window = sdl::window("Title", 800, 600);
        test_window_read_api(window);
    }
}

TEST_CASE("sdl::window_ref can be created from SDL_Window*",
          "[video][window]") {

    auto* c_win = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    auto window_ref = sdl::window_ref{c_win};
    test_window_read_api(window_ref);
    test_window_write_api(window_ref);
}

TEST_CASE("sdl::window_ref can be created from window id", "[video][window]") {
    auto* c_win = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    const auto window_ref = sdl::get_window_from_id(SDL_GetWindowID(c_win));
    test_window_read_api(window_ref);
}

TEST_CASE("sdl::window_ref can be created from sdl::window",
          "[video][window]") {

    auto window = sdl::window{"Title", 800, 600};
    const auto window_ref = sdl::window_ref{window};
    CHECK(window_ref);
    test_window_read_api(window_ref);
}

TEST_CASE("Screensaver can be enabled/disabled", "[video]") {
    SDL_EnableScreenSaver();
    CHECK(sdl::is_screensaver_enabled());
    SDL_DisableScreenSaver();
    CHECK_FALSE(sdl::is_screensaver_enabled());

    sdl::enable_screensaver();
    CHECK(SDL_IsScreenSaverEnabled() == SDL_TRUE);
    sdl::disable_screensaver();
    CHECK(SDL_IsScreenSaverEnabled() == SDL_FALSE);
}
