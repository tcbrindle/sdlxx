/*
  @file loadso.hpp
  System dependent library loading routines
  @copyright (C) 2016 Tristan Brindle <t.c.brindle@gmail.com>

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

#ifndef SDLXX_LOADSO_HPP
#define SDLXX_LOADSO_HPP

#include "SDL_loadso.h"

#include <memory>

#include "macros.hpp"

namespace sdl {

class shared_object_handle {

    template <typename...>
    struct function_ptr_helper;

    template <typename Ret, typename... Args>
    struct function_ptr_helper<Ret(Args...)> {
        using type = Ret (*)(Args...);
    };

    struct handle_deleter {
        void operator()(void* ptr) const { SDL_UnloadObject(ptr); }
    };

public:
    explicit shared_object_handle(const char* sofile)
        : handle(SDL_LoadObject(sofile)) {
        // On OS X at least, SDL_LoadObject(NULL) returns what appears to be a
        // valid pointer. This is probably not what we want, so let's just rule
        // out NULL now.
        SDLXX_CHECK(sofile);
        SDLXX_CHECK(handle);
    }

    template <typename Signature>
    auto load_function(const char* name) & {
        void* f = nullptr;
        SDLXX_CHECK(f = SDL_LoadFunction(handle.get(), name));
        using fp_t = typename function_ptr_helper<Signature>::type;
        return reinterpret_cast<fp_t>(f);
    }

    // For safety's sake, prevent this function begin called with
    // an rvalue (temporary) loader
    auto load_function(const char*)&& = delete;

    void* native_handle() const { return handle.get(); }

    explicit operator bool() const { return bool{handle}; }

private:
    std::unique_ptr<void, handle_deleter> handle = nullptr;
};
}

#endif // SDLXX_LOADSO_HPP
