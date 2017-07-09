#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

namespace js {

/**
 * \brief Get indices of random access container using a comparator.
 *
 * Given a comparator, the function returns a vector with the indicies
 * of the sorted random access container.
 */
template <class RandomAccessIterator, class Comparator>
std::vector<std::size_t> index_sort(RandomAccessIterator begin,
                                    RandomAccessIterator end, Comparator comp) {
    std::size_t size = std::distance(
        begin,
        end);  // Note, this makes only sense if begin < end and not end < begin
    std::vector<std::size_t> index_vec(size);
    std::iota(index_vec.begin(), index_vec.end(), static_cast<std::size_t>(0));
    std::sort(index_vec.begin(), index_vec.end(),
              [&](std::size_t i1, std::size_t i2) {
                  return comp(begin[i1], begin[i2]);
              });

    return index_vec;
}

/**
 * \brief Same as <index_sort> but with std::less as comparator
 *
 */
template <class RandomAccessIterator>
std::vector<std::size_t> index_sort(RandomAccessIterator begin,
                                    RandomAccessIterator end) {
    using type =
        typename std::iterator_traits<RandomAccessIterator>::value_type;
    return index_sort(begin, end, std::less<type>());
}

}  // end namespace js
