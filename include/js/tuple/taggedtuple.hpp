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

namespace js {

template <class Tag>
using tag_t = typename Tag::type;

template <class... Tag>
using tuple_t = std::tuple<tag_t<Tag>...>;

namespace detail {
template <class Tag, class... Tail>
struct FindTagIndex {};

template <class Tag, class... Tail>
struct FindTagIndex<Tag, Tag, Tail...>
    : std::integral_constant<std::size_t, 0> {};

template <class Tag, class Head, class... Tail>
struct FindTagIndex<Tag, Head, Tail...>
    : std::integral_constant<std::size_t,
                             FindTagIndex<Tag, Tail...>::value + 1> {};
}  // end namespace detail

/**\ingroup tuple
 * \brief Tagged tuple
 *
 * Tagged tuple is created using a tag of the from
 * \code
 * struct TagName {
 *      using type = type;
 *  };
 *  \endcode
 */
template <class... Arg>
struct TaggedTuple : std::tuple<tag_t<Arg>...> {
    TaggedTuple(tag_t<Arg>&&... arg)
        : std::tuple<tag_t<Arg>...>(std::forward<tag_t<Arg>>(arg)...) {}
    TaggedTuple(const tuple_t<Arg...>& tuple) : tuple_t<Arg...>(tuple) {}
    TaggedTuple(tuple_t<Arg...>&& tuple)
        : tuple_t<Arg...>(std::forward<tuple_t<Arg...>>(tuple)) {}
};

/**\ingroup tuple
 * \brief Get value of tag in TaggedTuple
 */
template <class Tag, class... Arg>
typename Tag::type& get(TaggedTuple<Arg...>& ttuple) {
    return std::get<detail::FindTagIndex<Tag, Arg...>::value>(ttuple);
}

}  // end namespace js
