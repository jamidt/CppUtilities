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

#include "std_extension.hpp"
#include <iterator>

#define HAS_METHOD(method)                                                   \
    template <class, class = void_t<>>                                       \
    struct has_method_##method : std::false_type {};                         \
    template <class T>                                                       \
    struct has_method_##method<T,                                            \
                               void_t<decltype(std::declval<T>().method())>> \
        : std::true_type {};

namespace js {
namespace detail {
HAS_METHOD(size)
HAS_METHOD(max_size)
HAS_METHOD(begin)
HAS_METHOD(cbegin)
HAS_METHOD(end)
HAS_METHOD(cend)
HAS_METHOD(empty)
}  // end namespace detail

/**\addtogroup type_traits
 * @{
 */

/**\defgroup container_concepts Container concepts
 * \brief Compile time checks for containers
 *
 * @{
 */
/// Has `size()` method with `T::size_type` return value
template <class T>
struct has_method_size
    : conjugation<detail::has_method_size<T>,
                  std::is_same<typename T::size_type,
                               decltype(std::declval<T>().size())>> {};

/// Has `max_size()` method with `T::size_type` return value
template <class T>
struct has_method_max_size
    : conjugation<detail::has_method_max_size<T>,
                  std::is_same<typename T::size_type,
                               decltype(std::declval<T>().max_size())>> {};

/// Has `empty()` method with to `bool` convertible return value
template <class T>
struct has_method_empty
    : conjugation<
          detail::has_method_empty<T>,
          std::is_convertible<decltype(std::declval<T>().empty()), bool>> {};

/**\brief Has `begin()` method with `T::iterator` or `T::const_iterator` return
 * value
 */
template <class T>
struct has_method_begin
    : conjugation<
          detail::has_method_begin<T>,
          disjunction<std::is_same<typename T::iterator,
                                   decltype(std::declval<T>().begin())>,
                      std::is_same<typename T::const_iterator,
                                   decltype(std::declval<T>().begin())>>> {};

/**\brief Has `end()` method with `T::iterator` or `T::const_iterator` return
 * value
 */
template <class T>
struct has_method_end
    : conjugation<
          detail::has_method_end<T>,
          disjunction<std::is_same<typename T::iterator,
                                   decltype(std::declval<T>().end())>,
                      std::is_same<typename T::const_iterator,
                                   decltype(std::declval<T>().end())>>> {};

/**\brief Has `cbegin()` method with `T::const_iterator` return
 * value
 */
template <class T>
struct has_method_cbegin
    : conjugation<detail::has_method_cbegin<T>,
                  std::is_same<typename T::const_iterator,
                               decltype(std::declval<T>().cbegin())>> {};

/**\brief Has `cend()` method with `T::const_iterator` return
 * value
 */
template <class T>
struct has_method_cend
    : conjugation<detail::has_method_cend<T>,
                  std::is_same<typename T::const_iterator,
                               decltype(std::declval<T>().cend())>> {};

/**\brief Has operator `==` and returns a value that is convertible to
 * bool
 */
template <class, class, class = void_t<>>
struct is_comparable : std::false_type {};

template <class T1, class T2>
struct is_comparable<T1, T2,
                     void_t<decltype(std::declval<T1>() == std::declval<T2>())>>
    : std::is_convertible<bool,
                          decltype(std::declval<T1>() == std::declval<T2>())> {
};

/**\brief Checks if type `T` has `T::iterator`
 */
template <class, class = void_t<>>
struct has_iterator : std::false_type {};

/**\cond
 */
template <class T>
struct has_iterator<T, void_t<typename T::iterator>>
    : std::is_same<typename T::difference_type,
                   typename std::iterator_traits<
                       typename T::iterator>::difference_type> {};
/**\endcond
 */

/**\brief Checks if `T` has `T::const_iterator`
 */
template <class, class = void_t<>>
struct has_const_iterator : std::false_type {};

/**\cond
 */
template <class T>
struct has_const_iterator<T, void_t<typename T::const_iterator>>
    : std::is_same<typename T::difference_type,
                   typename std::iterator_traits<
                       typename T::const_iterator>::difference_type> {};
/**\endcond
 */

/**\brief Checks if `T` has all container types
 *
 * * `T::value_type`
 * * `T::refrence`
 * * `T::size_type`
 * * `T::iterator`
 * * `T::const_iterator`
 */
template <class, class = void_t<>>
struct has_container_types : std::false_type {};

/**\cond
 */
template <class T>
struct has_container_types<T,
                           void_t<typename T::value_type, typename T::reference,
                                  typename T::size_type>>
    : conjugation<has_iterator<T>, has_const_iterator<T>> {};
/**\endcond
 */

/**\brief Check if all requirements of a container are fulfilled
 *
 * *Note*: Check for implementation of `std::swap` for the container has
 * to be checked. Use `std::swapable_with` in C++17 for this.
 */
template <class T>
struct is_container
    : conjugation<has_container_types<T>, std::is_default_constructible<T>,
                  std::is_copy_constructible<T>, std::is_assignable<T, T>,
                  std::is_destructible<T>, is_comparable<T, T>,
                  has_method_size<T>, has_method_max_size<T>,
                  has_method_begin<T>, has_method_end<T>, has_method_cbegin<T>,
                  has_method_cend<T>> {};

template <class T1, class T2>
constexpr bool is_comparable_v = is_comparable<T1, T2>::value;

template <class T>
constexpr bool is_negatable_v = is_negatable<T>::value;

template <class T>
constexpr bool has_iterator_v = has_iterator<T>::value;

template <class T>
constexpr bool has_const_iterator_v = has_const_iterator<T>::value;

template <class T>
constexpr bool is_container_v = is_container<T>::value;

///@}
/**@}
 */
}  // end namespace js

