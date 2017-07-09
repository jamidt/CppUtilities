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

#include <type_traits>
#include <utility>

namespace js {

namespace detail {
template <class>
struct has_emplace_back_t : std::true_type {};

template <class C, class... Arg>
static auto has_emplace_back_impl(int) -> has_emplace_back_t<
    decltype(std::declval<C>().emplace_back(std::declval<Arg>()...))>;

template <class, class...>
static auto has_emplace_back_impl(long) -> std::false_type;
}

/**
 * Check if the container has the emplace_back method.
 */
template <class C, class... Arg>
struct has_emplace_back
    : decltype(detail::has_emplace_back_impl<C, Arg...>(42)) {};

/**
 * This is only useful for non sequential containers. Check if the
 * return value is of the form std::pair<iterator, bool> (see std::set,
 * std::map...)
 */
template <class C, class... Arg>
struct has_pair_return : std::is_same<std::pair<typename C::iterator, bool>,
                                      decltype(std::declval<C>().emplace(
                                          std::declval<Arg>()...))> {};

// This is basically for all sequential container
template <class C, class... Arg>
auto select_return_iter(C& c, Arg&&... arg)
    -> std::enable_if_t<has_emplace_back<C, Arg...>::value,
                        typename C::iterator> {
    c.emplace_back(std::forward<Arg>(arg)...);
    return c.end();
}

// This is for containers that return std::pair<iterator, bool> by
// emplacing
template <class C, class Arg>
auto select_return_iter(C& c, Arg&& arg)
    -> std::enable_if_t<has_pair_return<C, Arg>::value, typename C::iterator> {
    return std::get<0>(c.emplace(std::forward<Arg>(arg)));
}

// This is for containers that return an iterator by emplacing
template <class C, class Arg>
auto select_return_iter(C& c, Arg&& arg) -> typename C::iterator {
    return c.emplace(std::forward<Arg>(arg));
}

}  // end namespace js
