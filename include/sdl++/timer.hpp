/*!
  @file timer.hpp
  Header for wrappers of the SDL time management routines.

  @copyright (C) 2014-2016 Tristan Brindle <t.c.brindle@gmail.com>

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

#ifndef SDLXX_TIMER_HPP
#define SDLXX_TIMER_HPP

#include "SDL_timer.h"

#include "utils.hpp"

#include <chrono>

namespace sdl {

struct clock {
    using rep = uint32_t;
    using period = std::milli;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<clock>;

    constexpr static bool is_steady = true;

    static time_point now() noexcept {
        return time_point{duration{SDL_GetTicks()}};
    }
};

using time_point = clock::time_point;
using duration = clock::duration;

inline uint64_t get_performance_counter() {
    return ::SDL_GetPerformanceCounter();
}

inline uint64_t get_performance_frequency() {
    return ::SDL_GetPerformanceFrequency();
}

inline void delay(duration interval) { ::SDL_Delay(interval.count()); }

using timer_callback_t = duration(duration);

namespace detail {

template <typename Func>
class timeout_t {
    static_assert(detail::check_signature<Func, timer_callback_t>::value,
                  "Supplied callback is not callable or does not match "
                  "expected type sdl::duration(sdl::duration)");

public:
    timeout_t(duration interval, Func callback)
        : callback(callback),
          id{::SDL_AddTimer(interval.count(), run_callback, this)} {
        SDLXX_CHECK(id != 0);
    }

    // Not default constructable, move-only
    timeout_t(timeout_t&&) = default;
    timeout_t& operator=(timeout_t&&) = default;

    ~timeout_t() { ::SDL_RemoveTimer(id); }

private:
    static uint32_t run_callback(uint32_t interval, void* param) {
        auto self = static_cast<timeout_t*>(param);
        return sdl::duration{self->callback(duration{interval})}.count();
    }

    Func callback;
    int id;
};

} // end namespace detail

template <typename Func>
detail::timeout_t<Func> make_timeout(duration interval, Func callback) {
    return detail::timeout_t<Func>{interval, callback};
}

} // end namespace sdl

#endif // SDLXX_TIMER_HPP
