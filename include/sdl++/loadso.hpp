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

#include "macros.hpp"
#include "stdinc.hpp"

#include <memory>

namespace sdl {

/*! @defgroup Loadso Shared Object Loading and Function Lookup

  This category contains classes functions for handling shared objects.

  @{
*/

/*!
 RAII handle representing a dynamic shared object.
 */
class shared_object_handle {
public:
    /*!
     Loads a dynamic shared object.

     @param sofile A system-specific filename to open
     @post If exceptions are enabled and not thrown, then the object is ready
           for use
     @throws sdl::error
    */
    explicit shared_object_handle(const char* sofile)
        : handle(SDL_LoadObject(sofile)) {
        // On OS X at least, SDL_LoadObject(NULL) returns what appears to be a
        // valid pointer. This is probably not what we want, so let's just rule
        // out NULL now.
        SDLXX_CHECK(sofile);
        SDLXX_CHECK(handle);
    }

    //! @copydoc shared_object_handle
    explicit shared_object_handle(const string& sofile)
        : shared_object_handle(sofile.c_str()) {}

    /*!
      Loads a function from the shared object

      @pre `bool(*this) == true`
      @returns A pointer to a function
      @throws sdl::error
     */
    void* load_function(const char* name) & {
        void* f = nullptr;
        SDLXX_CHECK(f = SDL_LoadFunction(handle.get(), name));
        return f;
    }

    //! This function is deleted to prevent you from loading a function using a
    //! loader which will shortly be destroyed
    void load_function(const char*)&& = delete;

    //! Returns a pointer to an OS-native DSO handle
    void* native_handle() const { return handle.get(); }

    //! Returns `true` if the object is in a valid state and can be used.
    //! If exceptions are enabled, this should always be true. If exceptions are
    //! not enabled, you *must* check this before using the object.
    explicit operator bool() const { return static_cast<bool>(handle); }

private:
    //!@cond
    struct handle_deleter {
        void operator()(void* ptr) const { SDL_UnloadObject(ptr); }
    };
    //!@endcond

    std::unique_ptr<void, handle_deleter> handle = nullptr;
};
//@}

} // end namespace sdl

#endif // SDLXX_LOADSO_HPP
