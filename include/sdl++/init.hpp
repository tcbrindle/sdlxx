/*!
  @file init.hpp
  Initialization and shutdown routines

  @copyright (C) 2014 Tristan Brindle <t.c.brindle@gmail.com>

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

#ifndef SDLXX_INIT_HPP
#define SDLXX_INIT_HPP

#include "utils.hpp"

#include "SDL.h"

#include <functional>
#include <initializer_list>
#include <numeric>

namespace sdl {

/*!
 @defgroup Init Initialization and Shutdown

 The functions in this category are used to set up SDL for use and generally
 have global effects in your program.

 SDL is made up of a number of different subsystems, each one of which must be
 initialized before it can be used.

 Three subsystems -- event handling, file I/O and threading -- are initialized
 automatically whenever any other systems are initialized. To initialize
 other subsystems you must do so explicitly.

 Initialization is preformed using *guard variables*. These perform
 initialization in their constructors and shutdown in their destructors, meaning
 that by placing a guard variable on the stack, you can never forget to shut
 down cleanly, even when an exception is thrown.

 General Usage
 -------------

 If you are writing a program which uses sdl++, the recommended way to
 initialize the library is to create an `sdl::init_guard` at the start of
 your `main()` routine, for example

 ```
 int main() {
     sdl::init_guard init{};

     // ... other code

     return 0;
 }
 ```

 The destructor of `init_guard` calls `SDL_Quit()` and performs final shutdown
 of *all* SDL subsystems (including those which were implicitly started), frees
 static memory buffers and fully cleans up after itself.

 You may be tempted to make an `init_guard` a static variable, so that its
 constructor is called before `main()` (and its destructor afterwards). Do not
 do this. The ordering of static constructors is not guaranteed in C++, and
 if you try to initialize SDL in this way there is no guarantee that the
 library will be initialized after another piece of state that it needs
 (for example OpenGL). It may work on your development system today, but on a
 different system or with a different compiler it may not. To be on the safe
 side, always place the guard variable inside `main()`.

 Note that `init_guard` variables cannot be copied, only moved. This is because
 the destructor shuts down all active subsystems, and so should only be called
 once.

 Special Cases
 -------------

 Whilst `init_guard` ensures that no SDL memory can be leaked, sometimes the
 behaviour (closing all active subsystems in its destructor) is not what is
 desired. This may be the case for example if you are writing a library which
 uses SDL. You need to ensure that the subsystems you need are initialized, but
 your users may still need other subsystems even after they are finished with
 your code.

 In this case, you should use `sdl::subsystem_init_guard`. It works just like
 `init_guard`, except that its destuctor doesn't shut down all systems, but
 only those which were explicitly initialized at construction. Unlike
 `init_guard`, `subsystem_init_guard` variables can be copied, as SDL itself
 reference-counts the number of times each subsystem is initialized and shut
 down.

 @{
 */

/*!
 Flags which may be passed to `sdl::init_guard`'s constructor

 These are the flags which may be passed to the `sdl::init_guard`
 constructor.  You should specify the subsystems which you will be
 using in your application.
 */
enum class init_flags {
    timer = SDL_INIT_TIMER, //!< The timer subsystem
    audio = SDL_INIT_AUDIO, //!< The audio subsystem
    video =
        SDL_INIT_VIDEO, //!< The video subsystem. Implies `init_flags::events`
    joystick = SDL_INIT_JOYSTICK, //!< The joystick subsystem. Implies
    //!`init_flags::events`
    haptic = SDL_INIT_HAPTIC, //!< The haptic feedback subsystem
    gamecontroller = SDL_INIT_GAMECONTROLLER, //!< The game controller
    //!< subsystem. Implies
    //!< `init_flags::joystick`
    events = SDL_INIT_EVENTS, //!< The events subsystem
    everything = SDL_INIT_EVERYTHING //!< Initialize all subsystems
};

namespace detail {
template <>
struct is_flags<init_flags> : std::true_type {};
}

//! @related init_flags
constexpr uint32_t unwrap(init_flags flags) {
    return static_cast<uint32_t>(flags);
}

/*!
 RAII initialization guard

 If you are writing an application using SDL, you should create an `init_guard`
 on the stack somewhere near the top of your `main()` routine, before calling
 any other SDL routines.

 This will ensure that SDL is correctly initialized, and the destructor of your
 guard variable will take care of shutting down the library correctly.

 */
struct init_guard {
    //! Creates an `init_guard` initializing all subsystems.
    //! @throws sdl::error
    init_guard() : init_guard(init_flags::everything) {}

    /*!
     Creates an `init_guard` initializing the subsystems given in `flags`.
     @param flags A list of subsystems given as members of the `init_flags`
                  enumeration joined with `operator|`, for example
                 `sdl::init_flags::video | sdl::init_flags::audio`
     @throws sdl::error
     */
    explicit init_guard(init_flags flags) {
        SDLXX_CHECK(::SDL_Init(unwrap(flags)) == 0);
    }

    /*!
     Creates an `init_guard` initializing the subsystems given in `flags_list`.
     For example

     ```
     sdl::init_guard guard{sdl::init_flags::video, sdl::init_flags::audio};
     ```

     @param flags_list A list of `init_flags`
     @throws std::error if exceptions are enabled
     */
    explicit init_guard(std::initializer_list<init_flags> flags_list) {
        init_flags flags = detail::ilist_to_flags(flags_list);
        ::SDL_Init(unwrap(flags));
    }

    //! Defaulted move constructor to prevent generation of a copy constructor
    init_guard(init_guard&&) noexcept = default;
    //! Defaulted move assignment to prevent generation of copy assignment
    init_guard& operator=(init_guard&&) noexcept = default;

    //! Shuts down all SDL subsystems
    ~init_guard() { ::SDL_Quit(); }
};

/*!
 RAII initialization guard for individual libraries
 */
class subsystem_init_guard {
    explicit subsystem_init_guard(init_flags flags) : c_flags{unwrap(flags)} {
        SDLXX_CHECK(::SDL_InitSubSystem(c_flags));
    }

    subsystem_init_guard(const subsystem_init_guard& other)
        : c_flags(other.c_flags) {
        ::SDL_InitSubSystem(c_flags);
    }

    subsystem_init_guard& operator=(const subsystem_init_guard& other) {
        auto tmp = other;
        std::swap(c_flags, tmp.c_flags);
        return *this;
    }

    subsystem_init_guard(subsystem_init_guard&& other) noexcept
        : c_flags(other.c_flags) {
        other.c_flags = 0;
    }

    subsystem_init_guard& operator=(subsystem_init_guard&& other) noexcept {
        std::swap(c_flags, other.c_flags);
        return *this;
    }

    ~subsystem_init_guard() { ::SDL_QuitSubSystem(c_flags); }

private:
    uint32_t c_flags = 0;
};

} // end namespace sdl

#endif // SDLXX_INIT_HPP
