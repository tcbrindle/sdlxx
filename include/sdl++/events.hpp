/*
  Simple DirectMedia Layer C++ Bindings
  Copyright (C) 2014 Tristan Brindle <t.c.brindle@gmail.com>

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

#include "stdinc.hpp"

namespace sdl {

namespace event_type {

enum event_type : uint32_t {
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

} // end namespace event_type

using event = variant<
    SDL_CommonEvent, SDL_WindowEvent, SDL_KeyboardEvent, SDL_TextEditingEvent,
    SDL_TextInputEvent, SDL_MouseMotionEvent, SDL_MouseButtonEvent,
    SDL_MouseWheelEvent, SDL_JoyAxisEvent, SDL_JoyBallEvent, SDL_JoyHatEvent,
    SDL_JoyButtonEvent, SDL_JoyDeviceEvent, SDL_ControllerAxisEvent,
    SDL_ControllerButtonEvent, SDL_ControllerDeviceEvent, SDL_AudioDeviceEvent,
    SDL_QuitEvent, SDL_UserEvent, SDL_SysWMEvent, SDL_TouchFingerEvent,
    SDL_MultiGestureEvent, SDL_DollarGestureEvent, SDL_DropEvent>;

inline event wrap(SDL_Event e) {
    switch (e.type) {
    case SDL_AUDIODEVICEADDED:
    case SDL_AUDIODEVICEREMOVED:
        return e.adevice;
    case SDL_CONTROLLERAXISMOTION:
        return e.caxis;
    case SDL_CONTROLLERBUTTONUP:
    case SDL_CONTROLLERBUTTONDOWN:
        return e.cbutton;
    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
    case SDL_CONTROLLERDEVICEREMAPPED:
        return e.cdevice;
    case SDL_DOLLARGESTURE:
    case SDL_DOLLARRECORD:
        return e.dgesture;
    case SDL_DROPFILE:
        return e.drop;
    case SDL_FINGERMOTION:
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
        return e.tfinger;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        return e.key;
    case SDL_JOYAXISMOTION:
        return e.jaxis;
    case SDL_JOYBALLMOTION:
        return e.jball;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        return e.jbutton;
    case SDL_JOYDEVICEADDED:
    case SDL_JOYDEVICEREMOVED:
        return e.jdevice;
    case SDL_MOUSEMOTION:
        return e.motion;
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
        return e.button;
    case SDL_MOUSEWHEEL:
        return e.wheel;
    case SDL_MULTIGESTURE:
        return e.mgesture;
    case SDL_QUIT:
        return e.quit;
    case SDL_SYSWMEVENT:
        return e.syswm;
    case SDL_TEXTEDITING:
        return e.edit;
    case SDL_USEREVENT:
        return e.user;
    case SDL_WINDOWEVENT:
        return e.window;
    default: // ???
        return e.common;
    }
}

namespace detail {

/// Define helper classes and function
template <typename ReturnT, typename... Lambdas>
struct lambda_visitor;

template <typename ReturnT, typename L1, typename... Lambdas>
struct lambda_visitor<ReturnT, L1, Lambdas...>
    : L1, lambda_visitor<ReturnT, Lambdas...> {
    using L1::operator();
    using lambda_visitor<ReturnT, Lambdas...>::operator();
    lambda_visitor(L1 l1, Lambdas... lambdas)
        : L1(l1), lambda_visitor<ReturnT, Lambdas...>(lambdas...) {}
};

template <typename ReturnT, typename L1>
struct lambda_visitor<ReturnT, L1> : L1 {
    using L1::operator();
    lambda_visitor(L1 l1) : L1(l1) {}
};

template <typename ReturnT>
struct lambda_visitor<ReturnT> {
    lambda_visitor() {}
};

template <typename ReturnT, typename... Lambdas>
lambda_visitor<ReturnT, Lambdas...> make_lambda_visitor(Lambdas... lambdas) {
    return {lambdas...};
}

} // end namespace detail

template <typename... Lambdas>
void apply(const event& e, Lambdas... lambdas) {
    return eggs::variants::apply(detail::make_lambda_visitor<void>(lambdas...),
                                 e);
}

namespace event_queue {

template <typename Func>
struct filter_cb_base {
    // static_assert(utils::check_signature<Func, int(event)>::value,
    //              "Supplied callback is not callable or does not match "
    //              "expected type int(event)");

    static int callback(void* data, SDL_Event* event) {
        auto self = static_cast<filter_cb_base*>(data);
        return self->func(*event);
    }

    filter_cb_base(Func func) : func(func) {}

    Func func;
};

template <typename Func>
struct filter_cb : private filter_cb_base<Func> {
    filter_cb(Func func) : filter_cb_base<Func>(func) {
        ::SDL_SetEventFilter(filter_cb_base<Func>::callback, this);
    }

    ~filter_cb() { ::SDL_SetEventFilter(nullptr, nullptr); }
};

template <typename Func>
struct watch_cb : private filter_cb_base<Func> {
    watch_cb(Func func) : filter_cb_base<Func>(func) {
        ::SDL_AddEventWatch(filter_cb_base<Func>::callback, this);
    }

    ~watch_cb() { ::SDL_DelEventWatch(filter_cb_base<Func>::callback, this); }
};

template <typename Func>
filter_cb<Func> set_filter(Func func) {
    return filter_cb<Func>(func);
}

template <typename Func>
watch_cb<Func> add_watch(Func func) {
    return watch_cb<Func>(func);
}

inline optional<event> poll() {
    SDL_Event e;
    return SDL_PollEvent(&e) ? optional<event>{wrap(e)} : nullopt;
}

// inline void push(const event e) {
// FIXME: SDLXX_CHECK here
//    ::SDL_PushEvent(const_cast<SDL_Event*>(&e));
//}

} // end namespace event_queue

} // end namespace sdl

#endif // SDLXX_EVENTS_HPP
