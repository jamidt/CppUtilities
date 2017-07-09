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

#include "iter_traits.hpp"
#include "iterator_tuple.hpp"
#include <iterator>
#include <tuple>
#include <type_traits>

namespace js {

template <class Head>
bool container_size(std::size_t& size, const Head& head) {
    size = head.size();
    return true;
}

// When called, this function returns true if all containers have the
// same size. Otherwise it returns false and the smallest size is given.
template <class Head, class... Tail>
bool container_size(std::size_t& size, const Head& head, const Tail&... tail) {
    bool tmp = container_size(size, tail...);
    std::size_t head_size = head.size();
    if (head_size != size) {
        if (head_size < size) size = head_size;
        return false;
    } else {
        return tmp;
    }
}

// Helper function for advancing iterator
template <class Iter>
Iter make_end(std::size_t length, Iter iter) {
    std::advance(iter, length);
    return iter;
}

template <class Head>
std::size_t calc_max_length(const Head& head) {
    return head.max_size();
}

// Get max length of Zip by getting the shortest max length of
// consisting containers
template <class Head, class... Tail>
std::size_t calc_max_length(const Head& head, const Tail&... tail) {
    std::size_t len = calc_max_length(tail...);
    std::size_t head_size = head.max_size();
    return head_size < len ? head_size : len;
}

template <class... Arg>
using ref_to_container = std::tuple<Arg&...>;

template <class F, class... Arg, std::size_t... I>
decltype(auto) create_iterator_from_tuple_imp(const std::tuple<Arg...>& t,
                                              F&& f,
                                              std::index_sequence<I...>) {
    return IteratorTuple<typename std::decay_t<Arg>::iterator...>(
        f(std::get<I>(t))...);
}

template <class F, class... Arg>
decltype(auto) create_iterator_from_tuple(const std::tuple<Arg...>& t, F&& f) {
    return create_iterator_from_tuple_imp(t, std::forward<F>(f),
                                          std::index_sequence_for<Arg...>{});
}

template <class C, class... Tuples, std::size_t... T>
decltype(auto) make_iter_from_tuples(std::index_sequence<T...>, C& c,
                                     Tuples&&... t) {
    return makeIteratorTuple(select_return_iter(c, std::forward<Tuples>(t))...);
}

/**\ingroup iterator
 * \brief Zips containers together
 *
 * Be careful, the containers **must** have the same size, the otherwise
 * the behavior is undefined.
 *
 * ###Issues:
 * * Zip does not fulfill the requirements for a container.
 * * Add random access if constructing container have random access.
 * * Range base for loops only with r-value ref
 */
template <class... Arg>
class ZipBase {
  public:
    using iterator = IteratorTuple<typename Arg::iterator...>;
    using const_iterator = IteratorTuple<typename Arg::const_iterator...>;
    using value_type = typename iterator::value_type;
    using difference_type = typename iterator::difference_type;
    using size_type = std::size_t;

  protected:
    ref_to_container<Arg...> _container;
    size_type _max_length;
    size_type _length;

  public:
    ZipBase() = delete;
    ZipBase(Arg&... arg)
        : _container(arg...), _max_length(calc_max_length(arg...)) {}
    ZipBase(const ZipBase<Arg...>& z)
        : _container(z._container),
          _max_length(z._max_length),
          _length(z._length) {}

    ref_to_container<Arg...> getContainerTuple() { return _container; }
    iterator begin() noexcept {
        return create_iterator_from_tuple(_container,
                                          [](auto& x) { return x.begin(); });
    }
    iterator end() noexcept {
        return create_iterator_from_tuple(_container,
                                          [](auto& x) { return x.end(); });
    }
    const_iterator cbegin() const noexcept {
        return create_iterator_from_tuple(
            _container, [](const auto& x) { return x.cbegin(); });
    }
    const_iterator cend() const noexcept {
        return create_iterator_from_tuple(
            _container, [](const auto& x) { return x.cend(); });
    }
    size_type size() const noexcept { return _length; }
    size_type max_size() const noexcept { return _max_length; }
    bool empty() const noexcept { return cbegin() == cend(); }
    template <class... C>
    friend bool operator==(const ZipBase<C...>&, const ZipBase<C...>&);
    template <class... C>
    friend bool operator!=(const ZipBase<C...>&, const ZipBase<C...>&);

    template <class... Tuples>
    iterator emplace(Tuples&&... t) {
        return make_iter_from_tuples(std::index_sequence<sizeof...(t)>{},
                                     _container, std::forward<Tuples>(t)...);
    }
};

template <class... Arg>
bool operator==(const ZipBase<Arg...>& z1, const ZipBase<Arg...>& z2) {
    return z1._container == z2._container;
}

template <class... Arg>
bool operator!=(const ZipBase<Arg...>& z1, const ZipBase<Arg...>& z2) {
    return z1._container != z2._container;
}

/**\ingroup iterator
 * \brief Creates Zip from given containers
 */
template <class... Arg>
decltype(auto) makeZip(Arg&... arg) {
    return ZipBase<std::decay_t<Arg>...>(arg...);
}

}  // end namespace zip
