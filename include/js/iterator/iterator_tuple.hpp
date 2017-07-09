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

#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>
#include <iterator>

namespace js {

namespace detail {
template <class... Iterator>
using reference_t = std::tuple<typename std::iterator_traits<Iterator>::reference...>;

template <class... Iter>
using iterator_t = std::tuple<Iter...>;

template <class F, class Iterator, std::size_t... I>
void iter_impl(F&& f, Iterator& iter, std::index_sequence<I...>) {
    int dummy[] = {0, (f(std::get<I>(iter)), 0)...};
    (void)dummy;
}

template <class Iterator, std::size_t... I>
decltype(auto) make_deref_impl(const Iterator& iter,
                               std::index_sequence<I...>) {
    return std::tie(*std::get<I>(iter)...);
}
}  // end namespace detail

/**@ingroup iterator
 * @brief Iterator tuple for Zip
 *
 * Note that this does not work with ranged based for loops of the form
 * \code{.cpp}
 * Zip<Containers...> zipper(cont...);
 * for(auto& x: zipped) { ... }
 * \endcode
 * because the dereferencing returns a r-valued tuple of the form
 *
 * std::tuple<typename Container::iterator::reference...>
 *
 * ###Issues:
 *  * This implementation makes needless and potentially time consuming
 *  copies of reference_t<Iter...> while dereferencing.
 *  * Note that cast from const_iterator to iterator is possible via
 *  copy constructor (FixMe)
 */
template <class... Iter>
class IteratorTuple {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<typename std::iterator_traits<Iter>::value_type...>;
    using reference = detail::reference_t<Iter...>;
    using pointer = std::tuple<typename std::iterator_traits<Iter>::pointer...>;

  private:
    detail::iterator_t<Iter...> _iter_pos;

  public:
    // Create from iterator tuple
    /*
    IteratorTuple(const std::tuple<Iter...>& tuple) : _iter_pos(tuple) {}
    IteratorTuple(std::tuple<Iter...>&& tuple) : _iter_pos(std::move(tuple)) {}
    */
    IteratorTuple(const Iter&... iter) : _iter_pos(iter...) {}
    IteratorTuple(Iter&&... iter) : _iter_pos(std::forward<Iter>(iter)...) {}

    IteratorTuple(const IteratorTuple<Iter...>& iter)
        : _iter_pos(iter._iter_pos) {}
    IteratorTuple(IteratorTuple<Iter...>&& iter)
        : _iter_pos(std::move(iter._iter_pos)) {}

    IteratorTuple<Iter...>& operator++() {
        detail::iter_impl([](auto& i) -> void { ++i; }, _iter_pos,
                          std::index_sequence_for<Iter...>{});
        return *this;
    }
    IteratorTuple<Iter...> operator++(int) {
        auto tmp = *this;
        detail::iter_impl([](auto& i) -> void { ++i; }, _iter_pos,
                          std::index_sequence_for<Iter...>{});
        return tmp;
    }
    IteratorTuple<Iter...>& operator--() {
        detail::iter_impl([](auto& i) -> void { --i; }, _iter_pos,
                          std::index_sequence_for<Iter...>{});
        return *this;
    }
    IteratorTuple<Iter...> operator--(int) {
        auto tmp = *this;
        detail::iter_impl([](auto& i) -> void { --i; }, _iter_pos,
                          std::index_sequence_for<Iter...>{});
        return tmp;
    }

    reference operator*() {
        return detail::make_deref_impl(_iter_pos,
                                       std::index_sequence_for<Iter...>{});
    }

    IteratorTuple<Iter...>& operator=(const IteratorTuple<Iter...>& iter) {
        _iter_pos = iter._iter_pos;
        return *this;
    }
    IteratorTuple<Iter...>& operator=(IteratorTuple<Iter...>&& iter) {
        _iter_pos = std::move(iter._iter_pos);
        return *this;
    }

    template <class... Iterator>
    friend bool operator==(const IteratorTuple<Iterator...>&,
                           const IteratorTuple<Iterator...>&);
    template <class... Iterator>
    friend bool operator!=(const IteratorTuple<Iterator...>&,
                           const IteratorTuple<Iterator...>&);
};

template <class... Iter>
bool operator==(const IteratorTuple<Iter...>& iter1,
                const IteratorTuple<Iter...>& iter2) {
    return iter1._iter_pos == iter2._iter_pos;
}

template <class... Iter>
bool operator!=(const IteratorTuple<Iter...>& iter1,
                const IteratorTuple<Iter...>& iter2) {
    return iter1._iter_pos != iter2._iter_pos;
}

/**\ingroup iterator
 * \brief Creates IteratorTuple from given iterators
 *
 * @param iter... iterators
 */
template <class... Iter>
decltype(auto) makeIteratorTuple(Iter&&... iter) {
    return IteratorTuple<std::decay_t<Iter>...>(std::forward<Iter>(iter)...);
}

}  // end namespace Zip
