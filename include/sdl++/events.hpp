/*
  @file event.hpp
  Simple DirectMedia Layer C++ Bindings
  copyright (C) 2014 Tristan Brindle <t.c.brindle@gmail.com>

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
#ifndef SDLXX_EVENTS_HPP
#define SDLXX_EVENTS_HPP

#include "SDL.h"

#include "detail/wrapper.hpp"
#include "macros.hpp"
#include "stdinc.hpp"
#include "timer.hpp"

namespace sdl {

/*!
  @defgroup Events Event Handling

  Some stuff about events

  @{
 */

enum class event_type : uint32_t {
    /* Application events */
    quit = SDL_QUIT, /**< User-requested quit */

    /* These application events have special meaning on iOS, see README-ios.txt
       for details */
    app_terminating =
        SDL_APP_TERMINATING, /**< The application is being terminated by the OS
                Called on iOS in applicationWillTerminate()
                Called on Android in onDestroy()
           */
    app_lowmemory = SDL_APP_LOWMEMORY, /**< The application is low on memory,
                            free memory if possible.
                            Called on iOS in
                            applicationDidReceiveMemoryWarning()
                            Called on Android in onLowMemory()
                       */
    app_willenterbackground = SDL_APP_WILLENTERBACKGROUND, /**< The application
                                     is about to enter the background
                                     Called on iOS in
                                     applicationWillResignActive()
                                     Called on Android in onPause()
                                */
    app_didenterbackground =
        SDL_APP_DIDENTERBACKGROUND, /**< The application did enter the
                background and may not get CPU for some time
                Called on iOS in applicationDidEnterBackground()
                Called on Android in onPause()
           */
    app_willenterforeground = SDL_APP_WILLENTERFOREGROUND, /**< The application
                                     is about to enter the foreground
                                     Called on iOS in
                                     applicationWillEnterForeground()
                                     Called on Android in onResume()
                                */
    app_didenterforeground =
        SDL_APP_DIDENTERFOREGROUND, /**< The application is now interactive
                Called on iOS in applicationDidBecomeActive()
                Called on Android in onResume()
           */

    /* Window events */
    windowevent = SDL_WINDOWEVENT, /**< Window state change */
    syswmevent = SDL_SYSWMEVENT, /**< System specific event */

    /* Keyboard events */
    keydown = SDL_KEYDOWN, /**< Key pressed */
    keyup = SDL_KEYUP, /**< Key released */
    textediting = SDL_TEXTEDITING, /**< Keyboard text editing (composition) */
    textinput = SDL_TEXTINPUT, /**< Keyboard text input */

    /* Mouse events */
    mousemotion = SDL_MOUSEMOTION, /**< Mouse moved */
    mousebuttondown = SDL_MOUSEBUTTONDOWN, /**< Mouse button pressed */
    mousebuttonup = SDL_MOUSEBUTTONUP, /**< Mouse button released */
    mousewheel = SDL_MOUSEWHEEL, /**< Mouse wheel motion */

    /* Joystick events */
    joyaxismotion = SDL_JOYAXISMOTION, /**< Joystick axis motion */
    joyballmotion = SDL_JOYBALLMOTION, /**< Joystick trackball motion */
    joyhatmotion = SDL_JOYHATMOTION, /**< Joystick hat position change */
    joybuttondown = SDL_JOYBUTTONDOWN, /**< Joystick button pressed */
    joybuttonup = SDL_JOYBUTTONUP, /**< Joystick button released */
    joydeviceadded = SDL_JOYDEVICEADDED, /**< A new joystick has been inserted
                                            into the system */
    joydeviceremoved =
        SDL_JOYDEVICEREMOVED, /**< An opened joystick has been removed */

    /* Game controller events */
    controlleraxismotion =
        SDL_CONTROLLERAXISMOTION, /**< Game controller axis motion */
    controllerbuttondown =
        SDL_CONTROLLERBUTTONDOWN, /**< Game controller button pressed */
    controllerbuttonup =
        SDL_CONTROLLERBUTTONUP, /**< Game controller button released */
    controllerdeviceadded = SDL_CONTROLLERDEVICEADDED, /**< A new Game
                                                          controller has been
                                                          inserted into the
                                                          system */
    controllerdeviceremoved = SDL_CONTROLLERDEVICEREMOVED, /**< An opened Game
                                                              controller has
                                                              been removed */
    controllerdeviceremapped =
        SDL_CONTROLLERDEVICEREMAPPED, /**< The controller mapping was updated */

    /* Touch events */
    fingerdown = SDL_FINGERDOWN,
    fingerup = SDL_FINGERUP,
    fingermotion = SDL_FINGERMOTION,

    /* Gesture events */
    dollargesture = SDL_DOLLARGESTURE,
    dollarrecord = SDL_DOLLARRECORD,
    multigesture = SDL_MULTIGESTURE,

    /* Clipboard events */
    clipboardupdate = SDL_CLIPBOARDUPDATE,

    /* Drag and drop events */
    dropfile = SDL_DROPFILE,

    /* Render events */
    render_targets_reset =
        SDL_RENDER_TARGETS_RESET, /**< The render targets have been reset */

    userevent = SDL_USEREVENT,

    _num_events = SDL_LASTEVENT
};

enum class button_state { pressed = SDL_PRESSED, released = SDL_RELEASED };

// C++ versions of the various SDL_*Event types

struct event_base {
    time_point timestamp;

    event_base(const SDL_Event& t)
        : timestamp{time_point{sdl::clock::duration{t.common.timestamp}}} {}
};

struct quit_event : event_base {
    using event_base::event_base;
};

struct app_terminating_event : event_base {
    using event_base::event_base;
};

struct app_lowmemory_event : event_base {
    using event_base::event_base;
};

struct app_willenterbackground_event : event_base {
    using event_base::event_base;
};

struct app_didenterbackground_event : event_base {
    using event_base::event_base;
};

struct app_willenterforeground_event : event_base {
    using event_base::event_base;
};

struct app_didenterforeground_event : event_base {
    using event_base::event_base;
};

struct window_event : event_base {
    uint32_t window_id;
    uint8_t event;
    int32_t data1;
    int32_t data2;

    window_event(const SDL_Event& e)
        : event_base{e},
          event{e.window.event},
          data1{e.window.data1},
          data2{e.window.data2} {}
};

struct key_event_base : event_base {
    uint32_t window_id;
    button_state state;
    bool repeat;
    SDL_Keysym keysym;

    key_event_base(const SDL_Event& e)
        : event_base{e},
          window_id{e.key.windowID},
          state{e.key.state == SDL_PRESSED ? button_state::pressed
                                           : button_state::released},
          repeat{e.key.repeat > 0},
          keysym{e.key.keysym} {}
};

struct keydown_event : key_event_base {
    using key_event_base::key_event_base;
};

struct keyup_event : key_event_base {
    using key_event_base::key_event_base;
};

struct text_input_event : event_base {
    uint32_t window_id; /**< The window with keyboard focus, if any */
    std::string text; /**< The editing text */

    text_input_event(SDL_Event& e)
        : event_base{e}, window_id{e.text.windowID}, text{e.text.text} {}
};

struct text_editing_event : event_base {
    uint32_t window_id; /**< The window with keyboard focus, if any */
    std::string text; /**< The editing text */
    int32_t start; /**< The start cursor of selected editing text */
    int32_t length; /**< The length of selected editing text */

    text_editing_event(SDL_Event& e)
        : event_base{e},
          window_id{e.edit.windowID},
          text{e.edit.text},
          start{e.edit.start},
          length{e.edit.length} {}
};

struct mouse_motion_event : event_base {
    uint32_t window_id; /**< The window with mouse focus, if any */
    uint32_t which; /**< The mouse instance id, or SDL_TOUCH_MOUSEID */
    button_state state; /**< The current button state */
    int32_t x; /**< X coordinate, relative to window */
    int32_t y; /**< Y coordinate, relative to window */
    int32_t xrel; /**< The relative motion in the X direction */
    int32_t yrel; /**< The relative motion in the Y direction */

    mouse_motion_event(const SDL_Event& e)
        : event_base{e},
          window_id{e.motion.windowID},
          state{e.motion.state == SDL_PRESSED ? button_state::pressed
                                              : button_state::released},
          x{e.motion.x},
          y{e.motion.y},
          xrel{e.motion.xrel},
          yrel{e.motion.yrel} {}
};

struct mouse_button_up_event : event_base {
    using event_base::event_base;
};

struct mouse_button_down_event : event_base {
    using event_base::event_base;
};

struct mouse_wheel_event : event_base {
    using event_base::event_base;
};

struct joy_axis_motion_event : event_base {
    using event_base::event_base;
};

struct joy_ball_motion_event : event_base {
    using event_base::event_base;
};

struct joy_hat_motion_event : event_base {
    using event_base::event_base;
};

struct joy_button_down_event : event_base {
    using event_base::event_base;
};

struct joy_button_up_event : event_base {
    using event_base::event_base;
};

struct joy_device_added_event : event_base {
    using event_base::event_base;
};

struct joy_device_removed_event : event_base {
    using event_base::event_base;
};

struct controller_axis_motion_event : event_base {
    using event_base::event_base;
};

struct controller_button_down_event : event_base {
    using event_base::event_base;
};

struct controller_button_up_event : event_base {
    using event_base::event_base;
};

struct controller_device_added_event : event_base {
    using event_base::event_base;
};

struct controller_device_removed_event : event_base {
    using event_base::event_base;
};

struct controller_device_remapped_event : event_base {
    using event_base::event_base;
};

struct finger_down_event : event_base {
    using event_base::event_base;
};

struct finger_up_event : event_base {
    using event_base::event_base;
};

struct finger_motion_event : event_base {
    using event_base::event_base;
};

struct dollar_gesture_event : event_base {
    using event_base::event_base;
};

struct dollar_record_event : event_base {
    using event_base::event_base;
};

struct multi_gesture_event : event_base {
    using event_base::event_base;
};

struct clipboard_update_event : event_base {
    using event_base::event_base;
};

struct drop_file_event : event_base {
    std::string file; /**< The file name */

    drop_file_event(SDL_Event& e)
        : event_base{e}, file{detail::from_c_value(e.drop.file)} {}
};

struct render_targets_reset_event : event_base {
    using event_base::event_base;
};

struct audio_device_added_event : event_base {
    using event_base::event_base;
};

struct audio_device_removed_event : event_base {
    using event_base::event_base;
};

struct syswm_event : event_base {
    using event_base::event_base;
};

struct user_event : event_base {
    using event_base::event_base;
};

using event =
    variant<quit_event, app_terminating_event, app_lowmemory_event,
            app_willenterbackground_event, app_didenterbackground_event,
            app_willenterforeground_event, app_didenterforeground_event,
            window_event, keydown_event, keyup_event, text_editing_event,
            text_input_event, mouse_motion_event, mouse_button_up_event,
            mouse_button_down_event, mouse_wheel_event, joy_axis_motion_event,
            joy_ball_motion_event, joy_hat_motion_event, joy_button_down_event,
            joy_button_up_event, joy_device_added_event,
            joy_device_removed_event, controller_axis_motion_event,
            controller_button_down_event, controller_button_up_event,
            controller_device_added_event, controller_device_removed_event,
            controller_device_remapped_event, finger_down_event,
            finger_up_event, finger_motion_event, dollar_gesture_event,
            dollar_record_event, multi_gesture_event, clipboard_update_event,
            drop_file_event, render_targets_reset_event,
            audio_device_added_event, audio_device_removed_event, syswm_event,
            user_event>;

inline event from_c_type(SDL_Event& e) {
    switch (e.type) {
    case SDL_AUDIODEVICEADDED:
        return audio_device_added_event{e};
    case SDL_AUDIODEVICEREMOVED:
        return audio_device_removed_event{e};
    case SDL_CONTROLLERAXISMOTION:
        return controller_axis_motion_event{e};
    case SDL_CONTROLLERBUTTONUP:
        return controller_button_up_event{e};
    case SDL_CONTROLLERBUTTONDOWN:
        return controller_button_down_event{e};
    case SDL_CONTROLLERDEVICEADDED:
        return controller_device_added_event{e};
    case SDL_CONTROLLERDEVICEREMOVED:
        return controller_device_removed_event{e};
    case SDL_CONTROLLERDEVICEREMAPPED:
        return controller_device_remapped_event{e};
    case SDL_DOLLARGESTURE:
        return dollar_gesture_event{e};
    case SDL_DOLLARRECORD:
        return dollar_record_event{e};
    case SDL_DROPFILE:
        return drop_file_event{e};
    case SDL_FINGERMOTION:
        return finger_motion_event{e};
    case SDL_FINGERDOWN:
        return finger_down_event{e};
    case SDL_FINGERUP:
        return finger_up_event{e};
    case SDL_KEYDOWN:
        return keydown_event{e};
    case SDL_KEYUP:
        return keyup_event{e};
    case SDL_JOYAXISMOTION:
        return joy_axis_motion_event{e};
    case SDL_JOYBALLMOTION:
        return joy_ball_motion_event{e};
    case SDL_JOYBUTTONDOWN:
        return joy_button_down_event{e};
    case SDL_JOYBUTTONUP:
        return joy_button_up_event{e};
    case SDL_JOYDEVICEADDED:
        return joy_device_added_event{e};
    case SDL_JOYDEVICEREMOVED:
        return joy_device_removed_event{e};
    case SDL_MOUSEMOTION:
        return mouse_motion_event{e};
    case SDL_MOUSEBUTTONUP:
        return mouse_button_up_event{e};
    case SDL_MOUSEBUTTONDOWN:
        return mouse_button_down_event{e};
    case SDL_MOUSEWHEEL:
        return mouse_wheel_event{e};
    case SDL_MULTIGESTURE:
        return multi_gesture_event{e};
    case SDL_QUIT:
        return quit_event{e};
    case SDL_SYSWMEVENT:
        return syswm_event{e};
    case SDL_TEXTEDITING:
        return text_editing_event{e};
    case SDL_TEXTINPUT:
        return text_input_event{e};
    case SDL_USEREVENT:
        return user_event{e};
    case SDL_WINDOWEVENT:
        return window_event{e};
    default: // ???
        SDL_assert(false);
        return user_event{e}; // Shut up, compiler
    }
}

namespace detail {

    // Define helper classes and function
    template <typename... Lambdas>
    struct lambda_visitor;

    template <typename L1, typename... Lambdas>
    struct lambda_visitor<L1, Lambdas...> : private L1,
                                            private lambda_visitor<Lambdas...> {
        using L1::operator();
        using lambda_visitor<Lambdas...>::operator();

        constexpr lambda_visitor(L1&& l1, Lambdas&&... lambdas)
            : L1(std::forward<L1>(l1)),
              lambda_visitor<Lambdas...>(std::forward<Lambdas>(lambdas)...) {}
    };

    template <typename L1>
    struct lambda_visitor<L1> : private L1 {
        using L1::operator();
        constexpr lambda_visitor(L1&& l1) : L1(std::forward<L1>(l1)) {}
    };

} // end namespace detail

template <typename Variant, typename... Lambdas>
constexpr decltype(auto) match(Variant&& variant, Lambdas&&... lambdas) {
    return visit(
        detail::lambda_visitor<std::decay_t<Lambdas>...>{
            std::forward<Lambdas>(lambdas)...},
        std::forward<Variant>(variant));
}

namespace detail {

    template <typename Func>
    struct event_filter_cb_base {
        // static_assert(utils::check_signature<Func, bool(event)>::value,
        //              "Supplied callback is not callable or does not match "
        //              "expected type bool(event)");

        static int callback(void* data, SDL_Event* event) {
            auto self = static_cast<event_filter_cb_base*>(data);
            return self->func(*event) ? 1 : 0;
        }

        event_filter_cb_base(Func func) : func(func) {}

        Func func;
    };

    template <typename Func>
    struct event_filter_cb : private event_filter_cb_base<Func> {
        event_filter_cb(Func func) : event_filter_cb_base<Func>(func) {
            ::SDL_SetEventFilter(event_filter_cb_base<Func>::callback, this);
        }

        ~event_filter_cb() { ::SDL_SetEventFilter(nullptr, nullptr); }
    };

    template <typename Func>
    struct event_watch_cb : private event_filter_cb_base<Func> {
        event_watch_cb(Func func) : event_filter_cb_base<Func>(func) {
            ::SDL_AddEventWatch(event_filter_cb_base<Func>::callback, this);
        }

        ~event_watch_cb() {
            ::SDL_DelEventWatch(event_filter_cb_base<Func>::callback, this);
        }
    };

} // end namespace detail

template <typename Func>
detail::event_filter_cb<Func> set_event_filter(Func func) {
    return detail::event_filter_cb<Func>(func);
}

template <typename Func>
detail::event_watch_cb<Func> add_event_watch(Func func) {
    return detail::event_watch_cb<Func>(func);
}

inline optional<event> poll_event() {
    SDL_Event e;
    return SDL_PollEvent(&e) ? optional<event>{from_c_type(e)} : nullopt;
}

inline event wait_event() {
    SDL_Event e;
    SDLXX_CHECK(SDL_WaitEvent(&e));
    return from_c_type(e);
}

template <typename T>
void enable_event(bool) {
    static_assert(sizeof(T) > 0,
                  "enable_event<T> can only be called with an SDL event type");
}

template <>
inline void enable_event<SDL_DropEvent>(bool enable) {
    SDL_EventState(SDL_DROPFILE, enable ? SDL_ENABLE : SDL_DISABLE);
}

// inline void push(const event e) {
// FIXME: SDLXX_CHECK here
//    ::SDL_PushEvent(const_cast<SDL_Event*>(&e));
//}

} // end namespace sdl

#endif // SDLXX_EVENTS_HPP
