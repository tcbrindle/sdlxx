/*!
  @file flags.hpp
  Simple DirectMedia Layer C++ Bindings
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

#ifndef SDLXX_DETAIL_FLAGS_HPP
#define SDLXX_DETAIL_FLAGS_HPP

#include <functional> // for bit_or
#include <initializer_list>
#include <numeric> // for accumulate
#include <type_traits>

namespace sdl {
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
//! @endcond

} // end namespace sdl

#endif
