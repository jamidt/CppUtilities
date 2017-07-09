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

#include "std_extension.hpp"
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>

namespace js {
/// Is sequence container
template <class C>
struct IsSequenceContainer : std::false_type {};

template <class T, class Allocator>
struct IsSequenceContainer<std::vector<T, Allocator>> : std::true_type {};

template <class T, std::size_t N>
struct IsSequenceContainer<std::array<T, N>> : std::true_type {};

template <class T, class Allocator>
struct IsSequenceContainer<std::list<T, Allocator>> : std::true_type {};

template <class T, class Allocator>
struct IsSequenceContainer<std::deque<T, Allocator>> : std::true_type {};

template <class T, class Allocator>
struct IsSequenceContainer<std::forward_list<T, Allocator>> : std::true_type {};

/// Constant Iterator
template <class C>
struct HasConstIterator : IsSequenceContainer<C> {};

/// Iterator
template <class C>
struct HasIterator : IsSequenceContainer<C> {};

/// Random Access
template <class C>
struct HasRandomAccess : std::false_type {};

template <class T, class Allocator>
struct HasRandomAccess<std::vector<T, Allocator>> : std::true_type {};

template <class T, std::size_t N>
struct HasRandomAccess<std::array<T, N>> : std::true_type {};

template <class T, class Allocator>
struct HasRandomAccess<std::deque<T, Allocator>> : std::true_type {};

}  // namespace js
