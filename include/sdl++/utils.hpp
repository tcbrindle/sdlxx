/*!
  @file utils.hpp
  Simple DirectMedia Layer C++ Bindings
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

#ifndef SDLXX_UTILS_HPP
#define SDLXX_UTILS_HPP

#include "SDL_assert.h"

#include <functional>
#include <numeric>
#include <stdexcept>
#include <type_traits>

namespace sdl {

//! Exception type thrown by sdl++ functions if exceptions are enabled.
//! Inherits from std::runtime_error.
struct error : std::runtime_error {
    //! Constructor
    error(const char* what) : std::runtime_error(what) {}
};

/*! @macro SDLXX_CHECK
 The `SDLXX_CHECK` macro is used internally to wrap SDL calls which return an
 error code.

 By default, if an SDL call fails, the wrapper will throw an exception of
 type `sdl::error`, containing the error details as returned by
 `SDL_GetError()`.

 Alternatively, if `SDLXX_NO_EXCEPTIONS` is defined, then the
 exception will instead become a call to `SDL_assert()`. The assertion system
 in SDL is extremely powerful and flexible, and works somewhat like an exception
 system of its own. It can do very clever things such as automatically showing
 a dialog on an assertion failure, and allowing you to retry the failed
 condition.

 The final alternative for ultimate flexibility is to define the `SDLXX_CHECK`
 macro yourself. If you do this however you must ensure that the condition
 is checked _EXACTLY ONCE_, otherwise you will certainly encounter problems.
 */
#ifndef SDLXX_CHECK
#ifndef SDLXX_NO_EXCEPTIONS
#define SDLXX_CHECK(condition)                                                 \
    do {                                                                       \
        if (!(condition)) throw sdl::error(SDL_GetError());                    \
    } while (SDL_NULL_WHILE_LOOP_CONDITION);
#else
#define SDLXX_CHECK(condition) SDL_assert(condition)
#endif
#endif

namespace detail {

template <typename EnumType>
struct is_flags : std::false_type {};

//! SFINAE-able type matching is_flags
template <typename EnumType>
using flags_check_t =
    typename std::enable_if_t<is_flags<EnumType>::value, void>;

//! Helper to convert an initializer list to OR'd flags
template <typename EnumType, typename = flags_check_t<EnumType>>
EnumType ilist_to_flags(std::initializer_list<EnumType> ilist) {
    return accumulate(cbegin(ilist), cend(ilist), static_cast<EnumType>(0),
                      std::bit_or<>{});
}

// The void_t trick
template <typename... T>
using void_t = void;

template <typename T>
using equality_comparison_t = decltype(std::declval<T>() == std::declval<T>());

template <typename T, typename = void>
struct is_equality_comparable : std::false_type {};

template <typename T>
struct is_equality_comparable<T, void_t<equality_comparison_t<T>>>
    : std::true_type {};

template <typename T>
using check_equality_comparable_t =
    std::enable_if_t<is_equality_comparable<T>::value, void>;

template <typename T>
using less_than_comparison_t = decltype(std::declval<T>() < std::declval<T>());

template <typename T, typename = void>
struct is_less_than_comparable : std::false_type {};

template <typename T>
struct is_less_than_comparable<T, void_t<less_than_comparison_t<T>>>
    : std::true_type {};

template <typename T>
using check_less_than_comparable_t =
    std::enable_if_t<is_less_than_comparable<T>::value, void>;

template <typename Func, typename Ret, typename... Args>
using check_signature_t =
    decltype(Ret{std::declval<Func>()(std::declval<Args>()...)});

template <typename, typename, typename = void>
struct check_signature : std::false_type {};

template <typename Func, typename Ret, typename... Args>
struct check_signature<Func, Ret(Args...),
                       void_t<check_signature_t<Func, Ret, Args...>>>
    : std::true_type {};

} // end namespace detail

//! @cond
template <typename EnumType, typename = detail::flags_check_t<EnumType>>
constexpr EnumType operator|(EnumType lhs, EnumType rhs) {
    using underlying = std::underlying_type_t<EnumType>;
    return static_cast<EnumType>(static_cast<underlying>(lhs) |
                                 static_cast<underlying>(rhs));
}

template <typename EnumType, typename = detail::flags_check_t<EnumType>>
constexpr EnumType operator|=(EnumType& lhs, EnumType rhs) {
    return lhs = lhs | rhs;
}

template <typename EnumType, typename = detail::flags_check_t<EnumType>>
constexpr EnumType operator&(EnumType lhs, EnumType rhs) {
    using underlying = std::underlying_type_t<EnumType>;
    return static_cast<EnumType>(static_cast<underlying>(lhs) &
                                 static_cast<underlying>(rhs));
}

template <typename EnumType, typename = detail::flags_check_t<EnumType>>
constexpr bool flag_is_set(EnumType flags, EnumType value) {
    using underlying = std::underlying_type_t<EnumType>;
    return static_cast<underlying>(flags & value) != 0;
}

template <typename T, typename = detail::check_equality_comparable_t<T>>
constexpr bool operator!=(const T& lhs, const T& rhs) {
    return !(lhs == rhs);
}

template <typename T, typename = detail::check_less_than_comparable_t<T>>
constexpr bool operator>(const T& lhs, const T& rhs) {
    return rhs < lhs;
}

template <typename T, typename = detail::check_less_than_comparable_t<T>>
constexpr bool operator<=(const T& lhs, const T& rhs) {
    return !(rhs < lhs);
}

template <typename T, typename = detail::check_less_than_comparable_t<T>>
constexpr bool operator>=(const T& lhs, const T& rhs) {
    return !(lhs < rhs);
}

//! @endcond

} // end namespace sdl

#endif // SDLXX_UTILS_HPP
