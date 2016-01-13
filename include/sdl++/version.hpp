/*
  Simple DirectMedia Layer C++ Bindings
  Copyright (C) 2016 Tristan Brindle <t.c.brindle@gmail.com>

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

#ifndef SDLXX_VERSION_HPP
#define SDLXX_VERSION_HPP

#include "SDL_version.h"

#include <iostream>

namespace sdl {

struct version : SDL_version {
    constexpr version(uint8_t major, uint8_t minor, uint8_t patch)
        : SDL_version{major, minor, patch} {}
};

inline constexpr bool operator==(const version& lhs, const version& rhs) {
    return lhs.major == rhs.major && lhs.minor == rhs.minor &&
           lhs.patch == rhs.patch;
}

inline constexpr bool operator!=(const version& lhs, const version& rhs) {
    return !(lhs == rhs);
}

inline constexpr bool operator<(const version& lhs, const version& rhs) {
    return lhs.major == rhs.major
               ? lhs.minor == rhs.minor ? lhs.patch < rhs.patch
                                        : lhs.minor < rhs.minor
               : lhs.major < rhs.major;
}

inline constexpr bool operator>(const version& lhs, const version& rhs) {
    return rhs < lhs;
}

inline constexpr bool operator<=(const version& lhs, const version& rhs) {
    return !(lhs > rhs);
}

inline constexpr bool operator>=(const version& lhs, const version& rhs) {
    return !(lhs < rhs);
}

inline std::ostream& operator<<(std::ostream& os, const version& v) {
    return os << int{v.major} << '.' << int{v.minor} << '.' << int{v.patch};
}

inline version get_linked_version() {
    version v{0, 0, 0};
    ::SDL_GetVersion(&v);
    return v;
}

inline constexpr version get_compiled_version() {
    return version{SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL};
}

inline const char* get_revision() { return ::SDL_GetRevision(); }

inline int get_revision_number() { return ::SDL_GetRevisionNumber(); }

} // end namespace sdl

#endif // SDLXX_VERSION_HPP
