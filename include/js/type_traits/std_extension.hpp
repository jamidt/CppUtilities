/*
CppUtility library
Copyright (C) 2016  Jan Schmidt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace js {
/*
 * Implement stuff that is in C++17 but not before.
 */
/**\ingroup type_traits
 */
template <bool B>
using bool_constant = std::integral_constant<bool, B>;

template <class... Arg>
struct make_void {
    using type = void;
};

/**\ingroup type_traits
 *
 * This is mainly used in SFINAE constructs.
 */
template <class... Arg>
using void_t = typename make_void<Arg...>::type;

template <class...>
struct conjugation : std::true_type {};

template <class Head>
struct conjugation<Head> : Head {};

template <class Head, class... Tail>
struct conjugation<Head, Tail...>
    : std::conditional_t<bool(Head::value), conjugation<Tail...>, Head> {};

template <class...>
struct disjunction : std::false_type {};

template <class Head>
struct disjunction<Head> : Head {};

template <class Head, class... Tail>
struct disjunction<Head, Tail...>
    : std::conditional_t<bool(Head::value), Head, disjunction<Tail...>> {};

template <class B>
struct negation : bool_constant<!B::value> {};

/**\brief Checks if `T` is integral type and if `t>0`
 */
//template <class T, T t>
//struct is_positive_integral
//    : conjugation<std::is_integral<T>, bool_constant<bool(t > 0)>> {};

/**\brief Is negatable and has to `bool` convertible return value
 */
template <class, class = void_t<>>
struct is_negatable : std::false_type {};

template <class T>
struct is_negatable<T, void_t<decltype(!std::declval<T>())>>
    : std::true_type {};

/**\brief Check if all conditions are fulfilled
 *
 * Checks if type T fulfills all conditions
 */
template <class T, template <class> class... Conditions>
struct fulfills : conjugation<Conditions<T>...> {};

// Use template variables to simplify usage.
// This only possible with >= C++14 compatible compilers
template <bool B>
constexpr bool bool_constant_v = bool_constant<B>::value;

template <class... B>
constexpr bool conjugation_v = conjugation<B...>::value;

template <class... B>
constexpr bool disjunction_v = disjunction<B...>::value;

template <class B>
constexpr bool negation_v = negation<B>::value;

template <class T>
constexpr bool is_integral_v = std::is_integral<T>::value;

template <class Tuple>
constexpr size_t tuple_size_v = std::tuple_size<Tuple>::value;

//template <class T, T t>
//constexpr bool is_positive_integral_v = is_positive_integral<T, t>::value;
/*
 * Note: this implementation is bogus. It doesn't perform checks!!! Pray
 * for C++17 :)
 */
template <class, class, class = void_t<>>
struct is_swapable_with : std::false_type {};

template <class T1, class T2>
struct is_swapable_with<
    T1, T2, void_t<decltype(swap(std::declval<T1>(), std::declval<T2>())),
                   decltype(swap(std::declval<T2>(), std::declval<T1>()))>>
    : std::true_type {};

template <class T>
struct is_swapable : is_swapable_with<T, T> {};

}  // end namespace js
