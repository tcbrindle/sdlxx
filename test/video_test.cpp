
#include <sdl++/video.hpp>

#include "SDL.h"

#include "catch.hpp"

using namespace std::string_literals;

namespace {

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

static_assert(sizeof(sdl::window) == sizeof(SDL_Window*), "");
static_assert(sizeof(sdl::window_ref) == sizeof(SDL_Window*), "");

template <typename Window>
void test_window_read_api(const Window& window) {
    REQUIRE(window);

    auto* c_win = sdl::to_c_value(window);

    REQUIRE(c_win != nullptr);
    REQUIRE(SDL_GetWindowTitle(c_win) == "Title"s);
    REQUIRE(window.get_title() == "Title"s);
    REQUIRE(window.get_display().get_index() ==
            SDL_GetWindowDisplayIndex(c_win));
    {
        SDL_DisplayMode mode;
        SDL_GetWindowDisplayMode(c_win, &mode);
        REQUIRE(window.get_display_mode() == sdl::from_c_value(mode));
    }
    REQUIRE(window.get_pixel_format() == SDL_GetWindowPixelFormat(c_win));
    REQUIRE(window.get_id() == SDL_GetWindowID(c_win));
    REQUIRE(static_cast<SDL_WindowFlags>(window.get_flags()) ==
            SDL_GetWindowFlags(c_win));
    {
        int x, y;
        SDL_GetWindowPosition(c_win, &x, &y);
        REQUIRE(window.get_position() == std::make_pair(x, y));
    }
    {
        int w, h;
        SDL_GetWindowSize(c_win, &w, &h);
        REQUIRE(window.get_size() == std::make_pair(w, h));
    }
    {
        int w, h;
        SDL_GetWindowMinimumSize(c_win, &w, &h);
        REQUIRE(window.get_minimum_size() == std::make_pair(w, h));
    }
    {
        int w, h;
        SDL_GetWindowMaximumSize(c_win, &w, &h);
        REQUIRE(window.get_maximum_size() == std::make_pair(w, h));
    }
    REQUIRE(window.get_brightness() == SDL_GetWindowBrightness(c_win));

    // Hmmm, this doesn't seem to work as advertised?
    SDL_ShowWindow(c_win);
    SDL_SetWindowGrab(c_win, SDL_TRUE);
    REQUIRE(window.get_grab() == SDL_GetWindowGrab(c_win));
    REQUIRE(sdl::get_grabbed_window() == SDL_GetGrabbedWindow());
    SDL_SetWindowGrab(c_win, SDL_FALSE);
}

template <typename Window>
void test_window_write_api(Window&& window) {
    REQUIRE(window);

    auto* c_win = sdl::to_c_value(window);
    REQUIRE(c_win != nullptr);

    {
        SDL_DisplayMode default_mode;
        SDL_GetWindowDisplayMode(c_win, &default_mode);

        sdl::display_mode in_mode{SDL_PIXELFORMAT_ARGB8888, 800, 600, 60};
        window.set_display_mode(in_mode);
        SDL_DisplayMode temp_mode;
        SDL_GetWindowDisplayMode(c_win, &temp_mode);
        REQUIRE(in_mode == sdl::from_c_value(temp_mode));

        window.set_default_display_mode();
        SDL_GetWindowDisplayMode(c_win, &temp_mode);
        REQUIRE(sdl::from_c_value(default_mode) ==
                sdl::from_c_value(temp_mode));
    }

    window.set_title("New Title");
    REQUIRE(SDL_GetWindowTitle(c_win) == "New Title"s);

    // FIXME: Set icon

    {
        window.set_position(50, 50);
        int x, y;
        SDL_GetWindowPosition(c_win, &x, &y);
        REQUIRE(x == 50);
        REQUIRE(y == 50);
    }

    {
        window.set_size(400, 300);
        int w, h;
        SDL_GetWindowSize(c_win, &w, &h);
        REQUIRE(w == 400);
        REQUIRE(h == 300);
    }

    {
        window.set_minimum_size(120, 100);
        int w, h;
        SDL_GetWindowMinimumSize(c_win, &w, &h);
        REQUIRE(w == 120);
        REQUIRE(h == 100);
    }

    {
        window.set_maximum_size(1600, 1200);
        int w, h;
        SDL_GetWindowMaximumSize(c_win, &w, &h);
        REQUIRE(w == 1600);
        REQUIRE(h == 1200);
    }

    // We have no real way to test these are doing what they say, but we can
    // at least check the functions can be compiled and called without error
    window.set_bordered(true);
    window.show();
    window.hide();
    window.raise();
    window.minimize();
    window.maximize();
    window.restore();
    REQUIRE(window.set_fullscreen(sdl::fullscreen_mode::fullscreen));
    REQUIRE(window.set_fullscreen(sdl::fullscreen_mode::fullscreen_desktop));
    REQUIRE(window.set_fullscreen(sdl::fullscreen_mode::windowed));

    window.set_grab(true);
    REQUIRE(SDL_GetWindowGrab(c_win) == SDL_TRUE);

    window.set_grab(false);
    REQUIRE(SDL_GetWindowGrab(c_win) == SDL_FALSE);

    window.set_brightness(0.5f);
    REQUIRE(SDL_GetWindowBrightness(c_win) == 0.5f);
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
    REQUIRE(window_ref);
    test_window_read_api(window_ref);
}

TEST_CASE("Screensaver can be enabled/disabled", "[video]") {
    SDL_EnableScreenSaver();
    REQUIRE(sdl::is_screensaver_enabled());
    SDL_DisableScreenSaver();
    REQUIRE_FALSE(sdl::is_screensaver_enabled());

    sdl::enable_screensaver();
    REQUIRE(SDL_IsScreenSaverEnabled() == SDL_TRUE);
    sdl::disable_screensaver();
    REQUIRE(SDL_IsScreenSaverEnabled() == SDL_FALSE);
}
