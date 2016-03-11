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

#include "stdinc.hpp"

#include <functional>
#include <numeric>
#include <type_traits>

namespace sdl {

namespace detail {
    inline string take_string(char* str) {
        string s(str);
        SDL_free(str);
        return s;
    }

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
    using equality_comparison_t =
        decltype(std::declval<T>() == std::declval<T>());

    template <typename T, typename = void>
    struct is_equality_comparable : std::false_type {};

    template <typename T>
    struct is_equality_comparable<T, void_t<equality_comparison_t<T>>>
        : std::true_type {};

    template <typename T>
    using check_equality_comparable_t =
        std::enable_if_t<is_equality_comparable<T>::value, void>;

    template <typename T>
    using less_than_comparison_t =
        decltype(std::declval<T>() < std::declval<T>());

    template <typename T, typename = void>
    struct is_less_than_comparable : std::false_type {};

    template <typename T>
    struct is_less_than_comparable<T, void_t<less_than_comparison_t<T>>>
        : std::true_type {};

    template <typename T>
    using check_less_than_comparable_t =
        std::enable_if_t<is_less_than_comparable<T>::value, void>;

    template <typename Func, typename Ret, typename... Args>
    using check_signature_t = std::is_convertible<
        decltype(std::declval<Func>()(std::declval<Args>()...)), Ret>;

    template <typename, typename, typename = void>
    struct check_signature : std::false_type {};

    template <typename Func, typename Ret, typename... Args>
    struct check_signature<Func, Ret(Args...),
                           void_t<check_signature_t<Func, Ret, Args...>>>
        : std::true_type {};

    template <typename CppType>
    struct c_type {
        using type = CppType;
    };

    template <typename CppType>
    using c_type_t = typename c_type<CppType>::type;

    template <typename T>
    auto to_c_value(T&& arg) {
        return static_cast<c_type_t<std::decay_t<T>>>(arg);
    }

    template <typename T>
    decltype(auto) from_c_value(T&& arg) {
        return std::forward<T>(arg);
    }

    struct void_return_tag {};
    struct value_return_tag {};

    template <typename CFunc, typename... CppArgs>
    auto do_c_call(value_return_tag, CFunc c_function, CppArgs&&... args) {
        return from_c_value(
            c_function(to_c_value(std::forward<CppArgs>(args))...));
    }

    template <typename CFunc, typename... CppArgs>
    void do_c_call(void_return_tag, CFunc c_function, CppArgs&&... args) {
        c_function(to_c_value(std::forward<CppArgs>(args))...);
    }

    template <typename CFunc, typename... CppArgs>
    auto c_call(CFunc c_function, CppArgs&&... args) {
        constexpr bool is_void_return =
            std::is_same<std::result_of_t<CFunc(decltype(to_c_value(args))...)>,
                         void>::value;
        using return_tag = std::conditional_t<is_void_return, void_return_tag,
                                              value_return_tag>;
        return do_c_call(return_tag{}, c_function,
                         std::forward<CppArgs>(args)...);
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
