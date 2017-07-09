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

namespace detail {
template <class UnitaryFunction, class Tuple, std::size_t... Index>
void for_each_tuple_impl(Tuple& tuple, UnitaryFunction f,
                         std::index_sequence<Index...>) {
    int dummy[] = {0, (f(std::get<Index>(tuple)), 0)...};
    (void)dummy;
}

template <class Function, class Tuple, std::size_t... Index>
decltype(auto) apply_tuple(Function&& f, Tuple&& tuple,
                           std::index_sequence<Index...>) {
    return f(std::get<Index>(tuple)...);
}

template <class Array, std::size_t... Index>
decltype(auto) tuple_from_array(Array&& array, std::index_sequence<Index...>) {
    return std::make_tuple(std::forward<Array>(array)[Index]...);
}

template <class F, class... Arg, std::size_t... I>
decltype(auto) functor_tuple(const std::tuple<Arg...>& t, F&& f,
                             std::index_sequence<I...>) {
    return std::make_tuple(std::forward<F>(f)(std::get<I>(t))...);
}

}  // end namespace detail

/**\ingroup tuple
 * \brief Apply unitary function to tuple arguments
 *
 * Apply unitary function to any argument in the tuple. The return value
 * of the function is ignored. Template functions can be used with C++14 by
 * using a template lambda function
 * \code{.cpp}
 * template<class T>
 * void f(T& x) { ... }
 * for_each_tuple(tuple, [](auto& x) { f(x); ]);
 * \endcode
 */
template <class UnitaryFunction, class Tuple>
void for_each_tuple(Tuple& tuple, UnitaryFunction f) {
    detail::for_each_tuple_impl(
        tuple, f, std::make_index_sequence<
                      std::tuple_size<std::decay_t<decltype(tuple)>>::value>{});
}

/**\ingroup tuple
 * \brief Forward tuple arguments to function.
 *
 */
template <class Function, class Tuple>
decltype(auto) apply_tuple(Function&& f, Tuple&& tuple) {
    return detail::apply_tuple(
        std::forward<Function>(f), std::forward<Tuple>(tuple),
        std::make_index_sequence<
            std::tuple_size<std::decay_t<decltype(tuple)>>::value>{});
}

/**\ingroup tuple
 * \brief Create a tuple from a given array
 *
 * Note this works only for >=C++14 because std::array is **not**
 * constexpr before.
 */
template <class Array>
decltype(auto) tuple_from_array(Array&& array) {
    return detail::tuple_from_array(std::forward<Array>(array),
                                    std::make_index_sequence<array.size()>{});
}

/**\ingroup tuple
 * \brief Create a new tuple from a given tuple by applying a function
 *
 * The function can be given in terms of a template lambda function to
 * match each argument.
 */
template <class F, class... Arg>
decltype(auto) functor_tuple(const std::tuple<Arg...>& t, F&& f) {
    return detail::functor_tuple(t, std::forward<F>(f),
                                 std::index_sequence_for<Arg...>{});
}

template <class Tuple>
struct ConstructFromTuple {
    ConstructFromTuple(const Tuple& tuple) : _tuple(tuple) {}
    template <class T>
    operator T() {
        return apply_tuple([](const auto&... arg) { return T(arg...); },
                           _tuple);
    }

  private:
    const Tuple& _tuple;
};

/**
 * \brief Construct object T from tuple.
 *
 * Returns object of type ConstructFromTuple, that can be cast to T and forward
 * arguments to constructor.
 */
template <class Tuple>
ConstructFromTuple<Tuple> construct_from_tuple(const Tuple& tuple) {
    return ConstructFromTuple<Tuple>(tuple);
}
}  // end namespace js
