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

template <typename EventType>
struct event_wrapper {
    using type = EventType;

    type operator()(const SDL_Event& e) {
        return type{wrap(e.common.timestamp)};
    }
};

template <typename EventType>
EventType wrap(const SDL_Event& e) {
    return event_wrapper<EventType>{}(e);
}

struct quit_event {
    time_point timestamp;
};

struct app_terminating_event {
    time_point timestamp;
};

struct app_lowmemory_event {
    time_point timestamp;
};

struct app_willenterbackground_event {
    time_point timestamp;
};

struct app_didenterbackground_event {
    time_point timestamp;
};

struct app_willenterforeground_event {
    time_point timestamp;
};

struct app_didenterforeground_event {
    time_point timestamp;
};

//! Window state change event data
struct window_event {
    time_point timestamp;
    Uint32 window_id; /**< The associated window */
    Uint8 event; /**< ::SDL_WindowEventID */
    Sint32 data1; /**< event dependent data */
    Sint32 data2; /**< event dependent data */
};

template <>
struct event_wrapper<window_event> {
    window_event operator()(const SDL_Event& e) {
        return window_event{wrap(e.window.timestamp), e.window.windowID,
                            e.window.event, e.window.data1, e.window.data2};
    }
};

inline SDL_WindowEvent unwrap(const window_event& e) {
    SDL_WindowEvent we;
    we.timestamp = unwrap(e.timestamp);
    we.windowID = e.window_id;
    we.event = e.event;
    we.data1 = e.data1;
    we.data2 = e.data2;
    return we;
}

struct syswm_event {
    time_point timestamp;
    // ...
};

struct keydown_event {
    time_point timestamp;
    uint32_t window_id;
    button_state state;
    bool repeat;
    SDL_Keysym keysym;
};

template <>
struct event_wrapper<keydown_event> {
    keydown_event operator()(const SDL_Event& e) {
        return keydown_event{wrap(e.key.timestamp), e.key.windowID,
                             static_cast<button_state>(e.key.state),
                             static_cast<bool>(e.key.repeat), e.key.keysym};
    }
};

struct keyup_event {
    time_point timestamp;
    uint32_t window_id;
    button_state state;
    bool repeat;
    SDL_Keysym keysym;
};

template <>
struct event_wrapper<keyup_event> {
    keyup_event operator()(const SDL_Event& e) {
        return keyup_event{wrap(e.key.timestamp), e.key.windowID,
                           static_cast<button_state>(e.key.state),
                           static_cast<bool>(e.key.repeat), e.key.keysym};
    }
};

struct textediting_event {
    time_point timestamp;
    uint32_t window_id;
    std::string text;
};

template <>
struct event_wrapper<textediting_event> {
    textediting_event operator()(const SDL_Event& e) {
        return {wrap(e.edit.timestamp), e.edit.windowID,
                std::string(e.edit.text + e.edit.start,
                            e.edit.text + e.edit.length)};
    }
};

struct textinput_event {
    time_point timestamp;
    uint32_t window_id;
    std::string text;
};

#if 0

#define SDL_TEXTEDITINGEVENT_TEXT_SIZE (32)
/**
 *  \brief Keyboard text editing event structure (event.edit.*)
 */
typedef struct SDL_TextEditingEvent
{
    Uint32 type;                                /**< ::SDL_TEXTEDITING */
    Uint32 timestamp;
    Uint32 windowID;                            /**< The window with keyboard focus, if any */
    char text[SDL_TEXTEDITINGEVENT_TEXT_SIZE];  /**< The editing text */
    Sint32 start;                               /**< The start cursor of selected editing text */
    Sint32 length;                              /**< The length of selected editing text */
} SDL_TextEditingEvent;

#define SDL_TEXTINPUTEVENT_TEXT_SIZE (32)
/**
 *  \brief Keyboard text input event structure (event.text.*)
 */
typedef struct SDL_TextInputEvent
{
    Uint32 type;                              /**< ::SDL_TEXTINPUT */
    Uint32 timestamp;
    Uint32 windowID;                          /**< The window with keyboard focus, if any */
    char text[SDL_TEXTINPUTEVENT_TEXT_SIZE];  /**< The input text */
} SDL_TextInputEvent;

/**
 *  \brief Mouse motion event structure (event.motion.*)
 */
typedef struct SDL_MouseMotionEvent
{
    Uint32 type;        /**< ::SDL_MOUSEMOTION */
    Uint32 timestamp;
    Uint32 windowID;    /**< The window with mouse focus, if any */
    Uint32 which;       /**< The mouse instance id, or SDL_TOUCH_MOUSEID */
    Uint32 state;       /**< The current button state */
    Sint32 x;           /**< X coordinate, relative to window */
    Sint32 y;           /**< Y coordinate, relative to window */
    Sint32 xrel;        /**< The relative motion in the X direction */
    Sint32 yrel;        /**< The relative motion in the Y direction */
} SDL_MouseMotionEvent;

/**
 *  \brief Mouse button event structure (event.button.*)
 */
typedef struct SDL_MouseButtonEvent
{
    Uint32 type;        /**< ::SDL_MOUSEBUTTONDOWN or ::SDL_MOUSEBUTTONUP */
    Uint32 timestamp;
    Uint32 windowID;    /**< The window with mouse focus, if any */
    Uint32 which;       /**< The mouse instance id, or SDL_TOUCH_MOUSEID */
    Uint8 button;       /**< The mouse button index */
    Uint8 state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
    Uint8 clicks;       /**< 1 for single-click, 2 for double-click, etc. */
    Uint8 padding1;
    Sint32 x;           /**< X coordinate, relative to window */
    Sint32 y;           /**< Y coordinate, relative to window */
} SDL_MouseButtonEvent;

/**
 *  \brief Mouse wheel event structure (event.wheel.*)
 */
typedef struct SDL_MouseWheelEvent
{
    Uint32 type;        /**< ::SDL_MOUSEWHEEL */
    Uint32 timestamp;
    Uint32 windowID;    /**< The window with mouse focus, if any */
    Uint32 which;       /**< The mouse instance id, or SDL_TOUCH_MOUSEID */
    Sint32 x;           /**< The amount scrolled horizontally, positive to the right and negative to the left */
    Sint32 y;           /**< The amount scrolled vertically, positive away from the user and negative toward the user */
    Uint32 direction;   /**< Set to one of the SDL_MOUSEWHEEL_* defines. When FLIPPED the values in X and Y will be opposite. Multiply by -1 to change them back */
} SDL_MouseWheelEvent;

/**
 *  \brief Joystick axis motion event structure (event.jaxis.*)
 */
typedef struct SDL_JoyAxisEvent
{
    Uint32 type;        /**< ::SDL_JOYAXISMOTION */
    Uint32 timestamp;
    SDL_JoystickID which; /**< The joystick instance id */
    Uint8 axis;         /**< The joystick axis index */
    Uint8 padding1;
    Uint8 padding2;
    Uint8 padding3;
    Sint16 value;       /**< The axis value (range: -32768 to 32767) */
    Uint16 padding4;
} SDL_JoyAxisEvent;

/**
 *  \brief Joystick trackball motion event structure (event.jball.*)
 */
typedef struct SDL_JoyBallEvent
{
    Uint32 type;        /**< ::SDL_JOYBALLMOTION */
    Uint32 timestamp;
    SDL_JoystickID which; /**< The joystick instance id */
    Uint8 ball;         /**< The joystick trackball index */
    Uint8 padding1;
    Uint8 padding2;
    Uint8 padding3;
    Sint16 xrel;        /**< The relative motion in the X direction */
    Sint16 yrel;        /**< The relative motion in the Y direction */
} SDL_JoyBallEvent;

/**
 *  \brief Joystick hat position change event structure (event.jhat.*)
 */
typedef struct SDL_JoyHatEvent
{
    Uint32 type;        /**< ::SDL_JOYHATMOTION */
    Uint32 timestamp;
    SDL_JoystickID which; /**< The joystick instance id */
    Uint8 hat;          /**< The joystick hat index */
    Uint8 value;        /**< The hat position value.
                         *   \sa ::SDL_HAT_LEFTUP ::SDL_HAT_UP ::SDL_HAT_RIGHTUP
                         *   \sa ::SDL_HAT_LEFT ::SDL_HAT_CENTERED ::SDL_HAT_RIGHT
                         *   \sa ::SDL_HAT_LEFTDOWN ::SDL_HAT_DOWN ::SDL_HAT_RIGHTDOWN
                         *
                         *   Note that zero means the POV is centered.
                         */
    Uint8 padding1;
    Uint8 padding2;
} SDL_JoyHatEvent;

/**
 *  \brief Joystick button event structure (event.jbutton.*)
 */
typedef struct SDL_JoyButtonEvent
{
    Uint32 type;        /**< ::SDL_JOYBUTTONDOWN or ::SDL_JOYBUTTONUP */
    Uint32 timestamp;
    SDL_JoystickID which; /**< The joystick instance id */
    Uint8 button;       /**< The joystick button index */
    Uint8 state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
    Uint8 padding1;
    Uint8 padding2;
} SDL_JoyButtonEvent;

/**
 *  \brief Joystick device event structure (event.jdevice.*)
 */
typedef struct SDL_JoyDeviceEvent
{
    Uint32 type;        /**< ::SDL_JOYDEVICEADDED or ::SDL_JOYDEVICEREMOVED */
    Uint32 timestamp;
    Sint32 which;       /**< The joystick device index for the ADDED event, instance id for the REMOVED event */
} SDL_JoyDeviceEvent;


/**
 *  \brief Game controller axis motion event structure (event.caxis.*)
 */
typedef struct SDL_ControllerAxisEvent
{
    Uint32 type;        /**< ::SDL_CONTROLLERAXISMOTION */
    Uint32 timestamp;
    SDL_JoystickID which; /**< The joystick instance id */
    Uint8 axis;         /**< The controller axis (SDL_GameControllerAxis) */
    Uint8 padding1;
    Uint8 padding2;
    Uint8 padding3;
    Sint16 value;       /**< The axis value (range: -32768 to 32767) */
    Uint16 padding4;
} SDL_ControllerAxisEvent;


/**
 *  \brief Game controller button event structure (event.cbutton.*)
 */
typedef struct SDL_ControllerButtonEvent
{
    Uint32 type;        /**< ::SDL_CONTROLLERBUTTONDOWN or ::SDL_CONTROLLERBUTTONUP */
    Uint32 timestamp;
    SDL_JoystickID which; /**< The joystick instance id */
    Uint8 button;       /**< The controller button (SDL_GameControllerButton) */
    Uint8 state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
    Uint8 padding1;
    Uint8 padding2;
} SDL_ControllerButtonEvent;


/**
 *  \brief Controller device event structure (event.cdevice.*)
 */
typedef struct SDL_ControllerDeviceEvent
{
    Uint32 type;        /**< ::SDL_CONTROLLERDEVICEADDED, ::SDL_CONTROLLERDEVICEREMOVED, or ::SDL_CONTROLLERDEVICEREMAPPED */
    Uint32 timestamp;
    Sint32 which;       /**< The joystick device index for the ADDED event, instance id for the REMOVED or REMAPPED event */
} SDL_ControllerDeviceEvent;

/**
 *  \brief Audio device event structure (event.adevice.*)
 */
typedef struct SDL_AudioDeviceEvent
{
    Uint32 type;        /**< ::SDL_AUDIODEVICEADDED, or ::SDL_AUDIODEVICEREMOVED */
    Uint32 timestamp;
    Uint32 which;       /**< The audio device index for the ADDED event (valid until next SDL_GetNumAudioDevices() call), SDL_AudioDeviceID for the REMOVED event */
    Uint8 iscapture;    /**< zero if an output device, non-zero if a capture device. */
    Uint8 padding1;
    Uint8 padding2;
    Uint8 padding3;
} SDL_AudioDeviceEvent;


/**
 *  \brief Touch finger event structure (event.tfinger.*)
 */
typedef struct SDL_TouchFingerEvent
{
    Uint32 type;        /**< ::SDL_FINGERMOTION or ::SDL_FINGERDOWN or ::SDL_FINGERUP */
    Uint32 timestamp;
    SDL_TouchID touchId; /**< The touch device id */
    SDL_FingerID fingerId;
    float x;            /**< Normalized in the range 0...1 */
    float y;            /**< Normalized in the range 0...1 */
    float dx;           /**< Normalized in the range -1...1 */
    float dy;           /**< Normalized in the range -1...1 */
    float pressure;     /**< Normalized in the range 0...1 */
} SDL_TouchFingerEvent;


/**
 *  \brief Multiple Finger Gesture Event (event.mgesture.*)
 */
typedef struct SDL_MultiGestureEvent
{
    Uint32 type;        /**< ::SDL_MULTIGESTURE */
    Uint32 timestamp;
    SDL_TouchID touchId; /**< The touch device index */
    float dTheta;
    float dDist;
    float x;
    float y;
    Uint16 numFingers;
    Uint16 padding;
} SDL_MultiGestureEvent;


/**
 * \brief Dollar Gesture Event (event.dgesture.*)
 */
typedef struct SDL_DollarGestureEvent
{
    Uint32 type;        /**< ::SDL_DOLLARGESTURE or ::SDL_DOLLARRECORD */
    Uint32 timestamp;
    SDL_TouchID touchId; /**< The touch device id */
    SDL_GestureID gestureId;
    Uint32 numFingers;
    float error;
    float x;            /**< Normalized center of gesture */
    float y;            /**< Normalized center of gesture */
} SDL_DollarGestureEvent;
#endif

/*!
 An event used to request a file open by the system (event.drop.*)
 This event is enabled by default, you can disable it with SDL_EventState().
 If this event is enabled, you must free the filename in the event.
 */
struct drop_event {
    time_point timestamp;
    std::string file;
};

inline drop_event wrap(SDL_DropEvent& d) {
    std::string s = d.file;
    SDL_free(d.file);
    return drop_event{wrap(d.timestamp), std::move(s)};
}

inline SDL_DropEvent unwrap(drop_event d) {
    return SDL_DropEvent{SDL_DROPFILE, unwrap(d.timestamp),
                         SDL_strdup(d.file.c_str())};
}

#if 0
/**
 *  \brief A user-defined event type (event.user.*)
 */
typedef struct SDL_UserEvent
{
    Uint32 type;        /**< ::SDL_USEREVENT through ::SDL_LASTEVENT-1 */
    Uint32 timestamp;
    Uint32 windowID;    /**< The associated window if any */
    Sint32 code;        /**< User defined event code */
    void *data1;        /**< User defined data pointer */
    void *data2;        /**< User defined data pointer */
} SDL_UserEvent;

#endif

using event = variant<
    window_event, syswm_event, keydown_event, keyup_event, textediting_event,
    textinput_event, SDL_MouseMotionEvent, SDL_MouseButtonEvent,
    SDL_MouseWheelEvent, SDL_JoyAxisEvent, SDL_JoyBallEvent, SDL_JoyHatEvent,
    SDL_JoyButtonEvent, SDL_JoyDeviceEvent, SDL_ControllerAxisEvent,
    SDL_ControllerButtonEvent, SDL_ControllerDeviceEvent, SDL_AudioDeviceEvent,
    quit_event, SDL_UserEvent, SDL_SysWMEvent, SDL_TouchFingerEvent,
    SDL_MultiGestureEvent, SDL_DollarGestureEvent, drop_event>;

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
        return wrap(e.drop);
    case SDL_FINGERMOTION:
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
        return e.tfinger;
    case SDL_KEYDOWN:
        return wrap<keydown_event>(e);
    case SDL_KEYUP:
        return wrap<keyup_event>(e);
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
        return wrap<quit_event>(e);
    case SDL_SYSWMEVENT:
        return e.syswm;
    case SDL_TEXTEDITING:
        return wrap<textediting_event>(e);
    case SDL_USEREVENT:
        return e.user;
    case SDL_WINDOWEVENT:
        return wrap<window_event>(e);
    default: // ???
        return wrap(e);
    }
}

namespace detail {

    // Define helper classes and function
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

} // end namespace detail

template <typename... Lambdas>
detail::lambda_visitor<void, Lambdas...>
    make_lambda_visitor(Lambdas... lambdas) {
    return {lambdas...};
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
    return SDL_PollEvent(&e) ? optional<event>{wrap(e)} : nullopt;
}

inline event wait_event() {
    SDL_Event e;
    SDLXX_CHECK(SDL_WaitEvent(&e));
    return wrap(e);
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
