/*
  @file video.hpp

  Header file for sdl++ video functions.

  @copyright (C) 2014 Tristan Brindle <t.c.brindle@gmail.com>
  @copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SDLXX_VIDEO_HPP
#define SDLXX_VIDEO_HPP

#include "SDL_video.h"

#include "detail/flags.hpp"
#include "detail/wrapper.hpp"
#include "macros.hpp"
#include "stdinc.hpp"

namespace sdl {

/*!
  @defgroup Video Display and Window Management

  This category contains functions for handling display and window actions.

  @{
*/

//! Flags passed to the `sdl::window` constructor
enum class window_flags : uint32_t {
    none = 0,
    //! Fullscreen window
    fullscreen = SDL_WINDOW_FULLSCREEN,
    //! Window usable with OpenGL
    opengl = SDL_WINDOW_OPENGL,
    //! Window is visible
    shown = SDL_WINDOW_SHOWN,
    //! Window is not visible
    hidden = SDL_WINDOW_HIDDEN,
    //! No window decoration
    borderless = SDL_WINDOW_BORDERLESS,
    //! Window can be resized
    resizable = SDL_WINDOW_RESIZABLE,
    //! Window can be minimized
    minimized = SDL_WINDOW_MINIMIZED,
    //! Window can be maximized
    maximized = SDL_WINDOW_MAXIMIZED,
    //! Window has grabbed input focus
    input_grabbed = SDL_WINDOW_INPUT_GRABBED,
    //! Window has input focus
    input_focus = SDL_WINDOW_INPUT_FOCUS,
    //! Window has mouse focus
    mouse_focus = SDL_WINDOW_MOUSE_FOCUS,
    //! Window is fullscreen in desktop mode
    fullscreen_desktop = SDL_WINDOW_FULLSCREEN_DESKTOP,
    //! Window not created by SDL
    foreign = SDL_WINDOW_FOREIGN,
    //! Window should be created in high-DPI mode if supported
    allow_highdpi = SDL_WINDOW_ALLOW_HIGHDPI,
    //! Window has mouse captured (unrelated to input_grabbed)
    mouse_capture = SDL_WINDOW_MOUSE_CAPTURE
};
namespace detail {
    template <>
    struct is_flags<window_flags> : std::true_type {};

    template <>
    struct c_type<window_flags> {
        using type = uint32_t;
    };
}

namespace windowpos {
    //! Used to indicate that you don't care what the window position is.
    constexpr int undefined = SDL_WINDOWPOS_UNDEFINED;
    //! Used to indicate that the window position should be centered.
    constexpr int centered = SDL_WINDOWPOS_CENTERED;
}

//! Enumeration values passed to `sdl::window::set_fullscreen()`
enum class fullscreen_mode {
    windowed = 0,
    fullscreen = SDL_WINDOW_FULLSCREEN,
    fullscreen_desktop = SDL_WINDOW_FULLSCREEN_DESKTOP
};

//! Returns the names of the video drivers which were compiled in to SDL
//! This function can be called at any time, include before `sdl::init`.
inline vector<string> get_video_drivers() {
    vector<string> output;
    int n = detail::c_call(::SDL_GetNumVideoDrivers);
    for (int i = 0; i < n; i++) {
        vector_append(output, string(detail::c_call(::SDL_GetVideoDriver, i)));
    }
    return output;
}

//! Returns the name of the currently initialized video driver, or `nullptr`
//! if video hasn't been initialized.
//! @sa sdl::video_init_guard
inline const char* get_current_video_driver() {
    return detail::c_call(::SDL_GetCurrentVideoDriver);
}

/*!
 RAII initialization guard for the video subsystem.

 This is only useful if you need to use a non-default video driver
 Otherwise it is called automatically by the `sdl::init_guard` or
 `sdl::subsystem_init_guard` constructors when passed the
 `sdl::init_flags::video` flag.

 @sa sdl::init_guard
 @sa sdl::subsystem_init_guard
*/
struct video_init_guard {
    //! Initializes the video subsystem with the default video driver
    video_init_guard() : video_init_guard(nullptr) {}

    //! Initializes the video subsystem using the given driver
    explicit video_init_guard(const char* driver_name) {
        SDLXX_CHECK(detail::c_call(::SDL_VideoInit, driver_name) == 0);
    }

    //! Initializes the video subsystem using the given driver
    explicit video_init_guard(const string& driver_name)
        : video_init_guard(driver_name.c_str()) {}

    //! Shuts down the video subsystem
    ~video_init_guard() { detail::c_call(::SDL_VideoQuit); }

    //! Defaulted move constructor to prevent copying
    video_init_guard(video_init_guard&&) = default;
    //! Defaulted move-assignment to prevent copying
    video_init_guard& operator=(video_init_guard&&) = default;
};

//! A structure that contains the description of a display mode.
// TODO: Remove constructors once MSVC supports NSDMIs for aggregates
struct display_mode {
    constexpr display_mode() = default;

    constexpr display_mode(uint32_t format, // FIXME: should be enum
                           int width, int height, int refresh_rate = 0,
                           void* driver_data = nullptr)
        : format{format},
          width{width},
          height{height},
          refresh_rate{refresh_rate},
          driver_data{driver_data} {}

    uint32_t format = 0;
    int width = 0;
    int height = 0;
    int refresh_rate = 0;
    void* driver_data = nullptr;
};

inline SDL_DisplayMode to_c_value(const display_mode& dm) {
    return SDL_DisplayMode{dm.format, dm.width, dm.height, dm.refresh_rate,
                           dm.driver_data};
}

inline display_mode from_c_value(const SDL_DisplayMode& dm) {
    return display_mode{dm.format, dm.w, dm.h, dm.refresh_rate, dm.driverdata};
}

//! Returns true if two display modes can be considered the same
inline bool operator==(const display_mode& lhs, const display_mode& rhs) {
    return std::tie(lhs.format, lhs.width, lhs.height, lhs.refresh_rate) ==
           std::tie(rhs.format, rhs.width, rhs.height, rhs.refresh_rate);
}

//! Represents a physical display connected to the machine
struct display {
    //! Constructs a `display` from the given index
    explicit display(int index) : index(index) {}

    //! Get the name of the display in UTF-8 encoding
    const char* get_name() const {
        return detail::c_call(::SDL_GetDisplayName, index);
    }

    //! Returns a list of all display modes available on this display
    vector<display_mode> get_modes() const {
        vector<display_mode> output;
        int n = detail::c_call(::SDL_GetNumDisplayModes, index);
        for (int i = 0; i < n; i++) {
            ::SDL_DisplayMode c_m;
            SDLXX_CHECK(detail::c_call(::SDL_GetDisplayMode, index, i, &c_m) ==
                        0);
            output.push_back(from_c_value(c_m));
        }
        return output;
    }

    //! Get information about the current display mode
    // FIXME: When does this fail?
    display_mode get_current_mode() const {
        ::SDL_DisplayMode m;
        SDLXX_CHECK(detail::c_call(::SDL_GetCurrentDisplayMode, index, &m) ==
                    0);
        return from_c_value(m);
    }

    //! Get information about the desktop display mode
    // FIXME: When does this fail?
    display_mode get_desktop_mode() const {
        ::SDL_DisplayMode m;
        SDLXX_CHECK(SDL_GetDesktopDisplayMode(index, &m) == 0);
        return from_c_value(m);
    }

    /*!
      Get the closest match to the requested display mode

      The available display modes are scanned, and the closest mode matching the
      requested mode is returned.  The mode format and refresh_rate default to
      the desktop mode if they are 0.  The modes are scanned with size being
      first priority, format being second priority, and finally checking the
      refresh_rate.  If all the available modes are too small, then `nullopt`
      is returned.

      @return The closest display mode if one was found, else `nullopt`
    */
    optional<display_mode>
    get_closest_mode(const display_mode& requested) const {
        SDL_DisplayMode found;
        const auto& req = to_c_value(requested);
        auto ret =
            detail::c_call(::SDL_GetClosestDisplayMode, index, &req, &found);
        if (ret != nullptr) {
            return from_c_value(found);
        } else {
            return nullopt;
        }
    }

    //! Returns a 3-tuple of the diagonal, horizontal and vertical DPIs of the
    //! display.
    //!
    //! It's easiest to use this function with `std::tie`, which also allows you
    //! to ignore fields you don't care about, for example
    //!
    //! ```cpp
    //! sdl::display disp;
    //! float horiz, vert;
    //! std::tie(std::ignore, horiz, vert) = disp.get_dpi();
    //! ```
    //!
    //! If DPI information is not available for the current display, this
    //! function will throw `sdl::error` if exceptions are enabled (and return
    //! `(0, 0, 0)` otherwise).
    //!
    //! @throws sdl::error
    std::tuple<float, float, float> get_dpi() const {
        float ddpi = 0;
        float hdpi = 0;
        float vdpi = 0;
        SDLXX_CHECK(detail::c_call(::SDL_GetDisplayDPI, index, &ddpi, &hdpi,
                                   &vdpi) == 0);
        return std::tuple<float, float, float>{ddpi, hdpi, vdpi};
    }

    //! Get the desktop area represented by the display, with the primary
    //! display located at `(0, 0)`.
    // FIXME: Needs modifying when we wrap SDL_Rect
    SDL_Rect get_bounds() const {
        SDL_Rect r = {0, 0, 0, 0};
        SDLXX_CHECK(detail::c_call(::SDL_GetDisplayBounds, index, &r) == 0);
        return r;
    }

    //! Returns the index of the current display, for debugging
    int get_index() const { return index; }

    //! Returns whether the current display is valid.
    //!
    //! If exceptions are enabled, this function should always return `true`.
    //! Otherwise, you *must* check this before calling any `display` member
    //! member functions.
    explicit operator bool() const { return index >= 0; }

private:
    int index;
};

//! Get all the displays currently available
//! Will return an empty vector if video has not been initialized
inline std::vector<display> get_displays() {
    vector<display> vec;
    int n = detail::c_call(::SDL_GetNumVideoDisplays);
    for (int i = 0; i < n; i++) { vector_append(vec, display{i}); }
    return vec;
}

namespace detail {

    template <typename Derived>
    struct window_api {

        //! Gets the display associated with the window
        display get_display() const {
            int index = detail::c_call(::SDL_GetWindowDisplayIndex, *this);
            SDLXX_CHECK(index >= 0);
            return display{index};
        }

        //! Resets the display mode to the default
        //!
        //! If exceptions are enabled, throws `sdl::error` if the mode could not
        //! be set.
        //!
        //! @throws sdl::error.
        void set_display_mode_default() {
            SDLXX_CHECK(detail::c_call(::SDL_SetWindowDisplayMode, *this,
                                       nullptr) == 0);
        }

        //! Set the display mode used when a fullscreen window is visible.
        //! By default the window's dimensions and the desktop format and
        //! refresh rate are used.
        //!
        //! If exceptions are enabled, throws `sdl::error` if the display mode
        //! could not be set.
        //!
        //! @throws sdl::error
        void set_display_mode(const display_mode& mode) {
            SDLXX_CHECK(
                detail::c_call(::SDL_SetWindowDisplayMode, *this, mode) == 0);
        }

        //! Gets the currently used display mode
        //!
        //! Equivalent to `get_display().get_current_mode()`
        //!
        //! @throws sdl::error
        display_mode get_display_mode() const {
            SDL_DisplayMode cdm;
            SDLXX_CHECK(
                detail::c_call(::SDL_GetWindowDisplayMode, *this, &cdm) == 0);
            return sdl::from_c_value(cdm);
        }

        //! Get the pixel format of this window
        // FIXME: Make enum
        uint32_t get_pixel_format() const {
            return detail::c_call(::SDL_GetWindowPixelFormat, *this);
        }

        //! Get the numeric ID of a window, for logging purposes
        uint32_t get_id() const {
            return detail::c_call(::SDL_GetWindowID, *this);
        }

        //! Get the window flags
        window_flags get_flags() const {
            return static_cast<window_flags>(
                detail::c_call(::SDL_GetWindowFlags, *this));
        }

        //! Set the title of the window, in UTF-8 format
        void set_title(const char* utf8) {
            detail::c_call(::SDL_SetWindowTitle, *this, utf8);
        }

        //! Set the title of the window, in UTF-8 format
        void set_title(string utf8) { set_title(utf8.c_str()); }

        //! Set the icon for a window
        // FIXME: Use wrapper for surface when we have one
        void set_icon(SDL_Surface* icon) {
            detail::c_call(::SDL_SetWindowIcon, *this, icon);
        }

        /*
         FIXME: Don't actually wrap these until we can do so in a type-safe way
         Perhaps std::experimental::any?
         */
        // void* set_window_data();
        // void* get_window_data();

        //! Sets the position of a window.
        //! @note The window coordinate origin is the upper left of the display
        void set_window_position(int x, int y) {
            detail::c_call(::SDL_SetWindowPosition, *this, x, y);
        }

        //! Gets the position of a window
        //! @note The window coordinate origin is the upper left of the display
        std::pair<int, int> get_window_position() const {
            int x = 0;
            int y = 0;
            detail::c_call(::SDL_GetWindowPosition, *this, &x, &y);
            return {x, y};
        }

        /*!
          Set the size of a window's client area.

          @note You can't change the size of a fullscreen window, it
          automatically
                 matches the size of the display mode.

          The window size in screen coordinates may differ from the size in
          pixels,
          if the window was created with `sdl::window_flags::allow_highdpi` on a
          platform with high-dpi support (e.g. iOS or OS X). Use
          `sdl::gl::get_drawable_size()` or `sdl::renderer::get_output_size()`
          to
          get the real client area size in pixels.
         */
        void set_size(int width, int height) {
            detail::c_call(::SDL_SetWindowSize, *this, width, height);
        }

        /*!
          Get the size of a window's client area.

          The window size in screen coordinates may differ from the size in
          pixels,
          if the window was created with `sdl::window_flags::allow_highdpi` on a
          platform with high-dpi support (e.g. iOS or OS X). Use
          `sdl::gl::get_drawable_size()` or `sdl::renderer::get_output_size()`
          to
          get the real client area size in pixels.
        */
        std::pair<int, int> get_size() const {
            int w = 0;
            int h = 0;
            detail::c_call(::SDL_GetWindowSize, *this, &w, &h);
            return {w, h};
        }

        //! Set the minimum size of a window's client area
        //! @note You can't change the minimum size of a fullscreen window, it
        //! automatically matches the size of the display mode.
        void set_minimum_size(std::pair<int, int> size) {
            detail::c_call(::SDL_SetWindowMinimumSize, *this, size.first,
                           size.second);
        }

        //! Get the minimum size of a window's client area, if set
        std::pair<int, int> get_minimum_size() const {
            int w = 0;
            int h = 0;
            detail::c_call(::SDL_GetWindowMinimumSize, *this, &w, &h);
            return {w, h};
        }

        //! Set the maximum size of a window's client area
        //! @note You can't change the maximum size of a fullscreen window, it
        //! automatically matches the size of the display mode.
        void set_maximum_size(std::pair<int, int> size) {
            detail::c_call(::SDL_SetWindowMaximumSize, *this, size.first,
                           size.second);
        }

        //! Get the maximum size of a window's client area, if set
        std::pair<int, int> get_maximum_size() const {
            int w = 0;
            int h = 0;
            detail::c_call(::SDL_GetWindowMaximumSize, *this, &w, &h);
            return {w, h};
        }

        /*!
          Set the border state of the window

          This will add or remove the window's `sdl::window_flags::borderless`
          flag
          and add or remove the border from th actual window. This is a no-op if
          the
          window's border already matches the requested state.

          @note You can't change the border state of a fullscreen window
         */
        void set_bordered(bool bordered) {
            detail::c_call(::SDL_SetWindowBordered, *this, bordered);
        }

        //! Shows the window
        void show() { detail::c_call(::SDL_ShowWindow, *this); }

        //! Hides the window
        void hide() { detail::c_call(::SDL_HideWindow, *this); }

        //! Raise the window above other windows and set the input focus
        void raise() { detail::c_call(::SDL_RaiseWindow, *this); }

        //! Make the window as large as possible
        void maximize() { detail::c_call(::SDL_MaximizeWindow, *this); }

        //! Minimize the window to an iconic representation
        void minimize() { detail::c_call(::SDL_MinimizeWindow, *this); }

        //! Restore the size and position of a minimized or maximized window
        void restore() { detail::c_call(::SDL_RestoreWindow, *this); }

        //! Sets the window's fullscreen state
        //! @returns `true` if the transition to the new mode was successful
        bool set_fullscreen(fullscreen_mode mode) {
            return detail::c_call(::SDL_SetWindowFullscreen, *this, mode) ==
                   SDL_TRUE;
        }

        /* N.B. These seem SDL 1.2-era and maybe not useful now, let's hold off
           wrapping them */
        // SDL_Surface* get_window_surface();
        // void update_window_surface();
        // void update_window_surface_rects();

        //! Set the window's input grab mode.
        //! If the caller enables a grab while another window is currently
        //! grabbed,
        //! the other window loses its grab in favour of the caller's window.
        void set_grab(bool grabbed) {
            detail::c_call(::SDL_SetWindowGrab, *this, grabbed);
        }

        //! Get the window's input grab mode
        bool get_grab() const {
            return detail::c_call(::SDL_GetWindowGrab, *this);
        }

        //! Sets the brightness (gamma correction) of the window
        //! @throws std::error
        // FIXME: Maybe just bool return here?
        void set_brightness(float brightness) {
            SDLXX_CHECK(detail::c_call(::SDL_SetWindowBrightness, *this,
                                       brightness) == 0);
        }

        //! Gets the brightness (gamma correction) of the window
        //! @returns The last brightness value passed to `set_brightness()`
        float get_brightness() {
            return detail::c_call(::SDL_GetWindowBrightness, *this);
        }

        // TODO: These are going to take a bit of thinking about
        // void set_gamma_ramp(?, ?, ?)
        // tuple<?, ?, ?> get_gamma_ramp()

        // TODO: Hittest callback machinery

        explicit operator bool() const { return to_c_value(*this) != nullptr; }

        friend SDL_Window* to_c_value(const window_api& self) {
            return to_c_value(static_cast<const Derived&>(self));
        }

    protected:
        ~window_api() = default;
    };

} // end namespace detail

//! An owned window
class window : public detail::window_api<window> {
public:
    /*!
      Create a window with the specified position, dimensions, and flags.

      @param title The title of the window, in UTF-8 encoding.
      @param x     The x position of the window, sdl::windowpos::centered, or
                     sdl::windowpos::undefined.
      @param y     The y position of the window, `sdl::windowpos::centered`, or
                    `sdl::windowpos::undefined`.
      @param w     The width of the window, in screen coordinates.
      @param h     The height of the window, in screen coordinates.
      @param flags The flags for the window

      @return A newly-created window.

      If the window is created with the SDL_WINDOW_ALLOW_HIGHDPI flag, its size
      in pixels may differ from its size in screen coordinates on platforms with
      high-DPI support (e.g. iOS and Mac OS X). Use SDL_GetWindowSize() to query
      the client area's size in screen coordinates, and SDL_GL_GetDrawableSize()
      or SDL_GetRendererOutputSize() to query the drawable size in pixels.
    */
    window(const char* title, int x, int y, int w, int h,
           window_flags flags = window_flags::none)
        : win(detail::c_call(::SDL_CreateWindow, title, x, y, w, h, flags)) {
        SDLXX_CHECK(win != nullptr);
    }

    /*!
     Create a window with the specified width, height and flags

     @param title The title of the window in UTF-8 encoding
     @param w     The width of the window, in screen coordinates
     @param h     The height of the window in screen coordinates
     @param flags The flags for the window

     A convenience overload for creating a window with `x` and `y` both set
     to `sdl::windowpos::undefined`.
     */
    window(const char* title, int w, int h,
           window_flags flags = window_flags::none)
        : window{title, windowpos::undefined, windowpos::undefined, w, h,
                 flags} {}

private:
    detail::c_ptr<SDL_Window, SDL_DestroyWindow> win = nullptr;

    friend SDL_Window* to_c_value(const window&);
};

inline SDL_Window* to_c_value(const window& w) { return w.win.get(); }

//! A lightweight view of an SDL window
//!
//! `window_view` is a view of a window that is owned elsewhere. The name is by
//! analogy to the (forthcoming) `string_view` and `array_view` standard library
//! classes.
//!
//! A `window_view` is a non-owning wrapper with (nullable) reference semantics.
class window_view : public detail::window_api<window_view> {
public:
    window_view(SDL_Window* win) : win(win) {}

    window_view(const window& win) : win(to_c_value(win)) {}

    window_view& operator=(SDL_Window* other) {
        // No point doing anything fancy
        win = other;
        return *this;
    }

    window_view& operator=(const window& other) {
        win = to_c_value(other);
        return *this;
    }

    //! @relates window_view
    friend SDL_Window* to_c_value(const window_view& w);

    //! @relates window_view
    friend window_view from_c_value(::SDL_Window* w);

private:
    ::SDL_Window* win = nullptr;
};

inline SDL_Window* to_c_value(const window_view& w) { return w.win; }

inline window_view from_c_value(::SDL_Window* w) { return window_view{w}; }

//! Get a `window_view` from a stored window id.
//! May return an invalid window if `id` does not exist
//!
//! @sa sdl::window::get_id()
window_view get_window_from_id(uint32_t id) {
    return detail::c_call(::SDL_GetWindowFromID, id);
}

//! Get the window that currently as an input grab enabled, if any
//!
//! @note This will return an empty view if no window has an input grab set
window_view get_grabbed_window() {
    return detail::c_call(::SDL_GetGrabbedWindow);
}

//! Whether or not the screensaver is currently enabled (default on).
bool is_screensaver_enabled() {
    return detail::c_call(::SDL_IsScreenSaverEnabled);
}

//! Allow the screen to be blanked by a screensaver
void enable_screensaver() { detail::c_call(::SDL_EnableScreenSaver); }

//! Prevent the screen from being blanked by a screensaver
void disable_screensaver() { detail::c_call(::SDL_DisableScreenSaver); }

/* OpenGL Support functions */

namespace gl {

    //! OpenGL configuration attributes
    enum class attribute {
        red_size = SDL_GL_RED_SIZE,
        green_size = SDL_GL_GREEN_SIZE,
        blue_size = SDL_GL_BLUE_SIZE,
        alpha_size = SDL_GL_ALPHA_SIZE,
        buffer_size = SDL_GL_BUFFER_SIZE,
        doublebuffer = SDL_GL_DOUBLEBUFFER,
        depth_size = SDL_GL_DEPTH_SIZE,
        stencil_size = SDL_GL_STENCIL_SIZE,
        accum_red_size = SDL_GL_ACCUM_RED_SIZE,
        accum_green_size = SDL_GL_ACCUM_GREEN_SIZE,
        accum_blue_size = SDL_GL_ACCUM_BLUE_SIZE,
        accum_alpha_size = SDL_GL_ACCUM_ALPHA_SIZE,
        stereo = SDL_GL_STEREO,
        multisamplebuffers = SDL_GL_MULTISAMPLEBUFFERS,
        multisamplesamples = SDL_GL_MULTISAMPLESAMPLES,
        accelerated_visual = SDL_GL_ACCELERATED_VISUAL,
        retained_backing = SDL_GL_RETAINED_BACKING,
        context_major_version = SDL_GL_CONTEXT_MAJOR_VERSION,
        context_minor_version = SDL_GL_CONTEXT_MINOR_VERSION,
        context_egl = SDL_GL_CONTEXT_EGL,
        context_flags = SDL_GL_CONTEXT_FLAGS,
        context_profile_mask = SDL_GL_CONTEXT_PROFILE_MASK,
        share_with_current_context = SDL_GL_SHARE_WITH_CURRENT_CONTEXT,
        framebuffer_srgb_capable = SDL_GL_FRAMEBUFFER_SRGB_CAPABLE,
        release_behavior = SDL_GL_CONTEXT_RELEASE_BEHAVIOR
    };

    //! Profile request flags
    enum class profile_flags {
        core = SDL_GL_CONTEXT_PROFILE_CORE,
        compatibility = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
        es = SDL_GL_CONTEXT_PROFILE_ES
    };

    //! Context flags
    enum class context_flags {
        debug = SDL_GL_CONTEXT_DEBUG_FLAG,
        forward_compatible = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG,
        robust_access = SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG,
        reset_isolation = SDL_GL_CONTEXT_RESET_ISOLATION_FLAG
    };

    //! Release flags
    enum class release_behavior_flags {
        none = SDL_GL_CONTEXT_RELEASE_BEHAVIOR_NONE,
        flush = SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH
    };

    // Annoyingly, we have to break out of our namespace in order to put some
    // things in namespace detail

} // end namespace gl

namespace detail {

    template <>
    struct is_flags<gl::profile_flags> : std::true_type {};

    template <>
    struct is_flags<gl::context_flags> : std::true_type {};

    template <>
    struct is_flags<gl::release_behavior_flags> : std::true_type {};

    template <>
    struct c_type<gl::attribute> {
        using type = SDL_GLattr;
    };

} // end namespace detail

/* back to */ namespace gl {

    //! RAII wrapper around a system OpenGL context
    //!
    //! As with other wrappers in *sdl++*, the context is created in the
    //! constructor and destroyed in the destructor.
    class context {
    public:
        //! Create an OpenGL context for use with an OpenGL window, and make it
        //! current.
        //!
        //! If exceptions are enabled, will throw if the context could not be
        //! created.
        //!
        //! @throws sdl::error
        context(window_view window)
            : c_context(
                  detail::c_call(::SDL_GL_CreateContext, to_c_value(window))) {
            SDLXX_CHECK(c_context);
        }

        //! Returns `true` if the current context was created correctly.
        //! If exceptions are disabled, you must check this before using the
        //! context
        explicit operator bool() const { return bool(c_context); }

    private:
        detail::c_ptr<void, SDL_GL_DeleteContext> c_context = nullptr;

        friend class context_view;
    };

    //! Lightweight wrapper around an `sdl::gl::context`
    class context_view {
    public:
        //! Default constructor, initializes to a null view
        context_view() = default;

        //! Creates a `context_view` from a C `SDL_GLContext`
        context_view(::SDL_GLContext c_context) : c_context(c_context) {}

        //! Creates a `context_view` from an owned `sdl::gl::context`
        context_view(const context& other) : c_context(other.c_context.get()) {}

        //! Returns `true` if the view
        explicit operator bool() const { return c_context != nullptr; }

        friend ::SDL_GLContext to_c_value(const context_view& cv) {
            return cv.c_context;
        }

    private:
        ::SDL_GLContext c_context = nullptr;
    };

    /**
     *  \brief Dynamically load an OpenGL library.
     *
     *  \param path The platform dependent OpenGL library name, or NULL to open
     * the
     *              default OpenGL library.
     *
     *  \return 0 on success, or -1 if the library couldn't be loaded.
     *
     *  This should be done after initializing the video driver, but before
     *  creating any OpenGL windows.  If no OpenGL library is loaded, the
     * default
     *  library will be loaded upon creation of the first OpenGL window.
     *
     *  \note If you do this, you need to retrieve all of the GL functions used
     * in
     *        your program from the dynamic library using
     * SDL_GL_GetProcAddress().
     *
     *  \sa SDL_GL_GetProcAddress()
     *  \sa SDL_GL_UnloadLibrary()
     */
    inline int load_library(const char* path) {
        return detail::c_call(::SDL_GL_LoadLibrary, path);
    }

    /**
     *  \brief Get the address of an OpenGL function.
     */
    inline void* get_proc_address(const char* proc) {
        return detail::c_call(::SDL_GL_GetProcAddress, proc);
    }

    /**
     *  \brief Unload the OpenGL library previously loaded by
     * SDL_GL_LoadLibrary().
     *
     *  \sa SDL_GL_LoadLibrary()
     */
    inline void unload_library() { detail::c_call(::SDL_GL_UnloadLibrary); }

    /**
     *  \brief Return true if an OpenGL extension is supported for the current
     *         context.
     */
    inline bool extension_supported(const char* ext) {
        return detail::c_call(::SDL_GL_ExtensionSupported, ext) == SDL_TRUE;
    }

    /**
     *  \brief Reset all previously set OpenGL context attributes to their
     * default values
     */
    inline void reset_attributes() {
        return detail::c_call(::SDL_GL_ResetAttributes);
    }

    /**
     *  \brief Set an OpenGL window attribute before window creation.
     */
    template <typename T, typename unused = std::enable_if_t<
                              std::is_same<T, int>::value ||
                              std::is_same<T, profile_flags>::value ||
                              std::is_same<T, context_flags>::value ||
                              std::is_same<T, release_behavior_flags>::value>>
    inline int set_attribute(attribute attr, T value) {
        return detail::c_call(::SDL_GL_SetAttribute, attr, value);
    }

    /**
     *  \brief Get the actual value for an attribute from the current context.
     */
    inline int get_attribute(attribute attr, int& value) {
        return detail::c_call(::SDL_GL_GetAttribute, attr, &value);
    }

    /**
     *  \brief Set up an OpenGL context for rendering into an OpenGL window.
     *
     *  \note The context must have been created with a compatible window.
     */
    int make_current(window_view w, context_view c) {
        return detail::c_call(::SDL_GL_MakeCurrent, w, c);
    }

    /**
     *  \brief Get the currently active OpenGL window.
     */
    inline window_view get_current_window() {
        return detail::c_call(::SDL_GL_GetCurrentWindow);
    }

    /**
     *  \brief Get the currently active OpenGL context.
     */
    inline context_view get_current_context() {
        return detail::c_call(::SDL_GL_GetCurrentContext);
    }

    /**
     *  \brief Get the size of a window's underlying drawable (for use with
     * glViewport).
     *
     *  \param window   Window from which the drawable size should be queried
     *  \param w        Pointer to variable for storing the width, may be NULL
     *  \param h        Pointer to variable for storing the height, may be NULL
     *
     * This may differ from SDL_GetWindowSize if we're rendering to a high-DPI
     * drawable, i.e. the window was created with SDL_WINDOW_ALLOW_HIGHDPI on a
     * platform with high-DPI support (Apple calls this "Retina"), and not
     * disabled
     * by the SDL_HINT_VIDEO_HIGHDPI_DISABLED hint.
     *
     *  \sa SDL_GetWindowSize()
     *  \sa SDL_CreateWindow()
     */
    template <typename Window>
    std::pair<int, int> get_drawable_size(Window& win) {
        int w, h;
        detail::c_call(::SDL_GL_GetDrawableSize, static_cast<SDL_Window*>(win),
                       &w, &h);
        return {w, h};
    }

    enum class swap_interval : int {
        immediate = 0,
        synchronized = 1,
        allow_late = -1
    };

    /**
     *  \brief Set the swap interval for the current OpenGL context.
     *
     *  \param interval 0 for immediate updates, 1 for updates synchronized with
     * the
     *                  vertical retrace. If the system supports it, you may
     *                  specify -1 to allow late swaps to happen immediately
     *                  instead of waiting for the next retrace.
     *
     *  \return true on success, or false if setting the swap interval is not
     * supported.
     *
     *  \sa SDL_GL_GetSwapInterval()
     */
    inline bool set_swap_interval(swap_interval interval) {
        return (detail::c_call(::SDL_GL_SetSwapInterval,
                               static_cast<int>(interval)) == 0);
    }

    /**
     *  \brief Get the swap interval for the current OpenGL context.
     *
     *  \return 0 if there is no vertical retrace synchronization, 1 if the
     * buffer
     *          swap is synchronized with the vertical retrace, and -1 if late
     *          swaps happen immediately instead of waiting for the next
     * retrace.
     *          If the system can't determine the swap interval, or there isn't
     * a
     *          valid current context, this will return 0 as a safe default.
     *
     *  \sa SDL_GL_SetSwapInterval()
     */
    inline swap_interval get_swap_interval() {
        return static_cast<swap_interval>(
            detail::c_call(::SDL_GL_GetSwapInterval));
    }

    /**
     * \brief Swap the OpenGL buffers for a window, if double-buffering is
     *        supported.
     */
    void swap_window(window_view win) {
        detail::c_call(::SDL_GL_SwapWindow, win);
    }

} // end namespace gl

} // end namespace sdl

#endif // SDL2_CPP_WINDOW_HPP
