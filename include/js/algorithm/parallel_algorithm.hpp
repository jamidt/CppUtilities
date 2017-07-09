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

/*
 * Be advised, this is mainly for test purposes to see if there is any
 * difference between using the raw std::thread and std::async approach.
 */


#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <thread>
#include <vector>
#include <list>
#include <future>

namespace js {
namespace detail {
template <class Iterator, class BinaryOperator, class T>
struct accumulate {
    void operator()(Iterator start, Iterator stop, T& out, BinaryOperator op) {
        out = *start;
        ++start;
        out = std::accumulate(start, stop, out, op);
    }
};
template <class Iterator, class Functor>
struct for_each {
    void operator()(Iterator start, Iterator stop, Functor f) {
        std::for_each(start, stop, f);
    }
};
}  // end detail

template <class Iterator, class BinaryOperator, class T>
T parallel_accumulate(Iterator begin, Iterator end, T init, BinaryOperator op,
                      std::size_t no_threads) {
    std::size_t length = std::distance(begin, end);
    std::size_t block_size = length / no_threads;
    std::vector<std::thread> threads;
    std::vector<T> tmp_acc(no_threads, 0);
    auto block_start = begin;
    for (std::size_t i = 0; i != (no_threads - 1); ++i) {
        auto block_end = block_start;
        std::advance(block_end, block_size);
        threads.emplace_back(
            std::move(detail::accumulate<Iterator, BinaryOperator, T>()),
            block_start, block_end, std::ref(tmp_acc[i]), op);
        block_start = block_end;
    }
    threads.emplace_back(
        std::move(detail::accumulate<Iterator, BinaryOperator, T>()),
        block_start, end, std::ref(tmp_acc[no_threads - 1]), op);
    for (auto& t : threads) {
        t.join();
    }
    return std::accumulate(tmp_acc.begin(), tmp_acc.end(), init, op);
}

template <class Iterator, class T>
T parallel_accumulate(Iterator begin, Iterator end, T init,
                      std::size_t no_threads) {
    return parallel_accumulate(begin, end, std::plus<T>(), init, no_threads);
}

template <class Iterator, class T, class BinaryOperator>
T parallel_accumulate_auto(Iterator begin, Iterator end, T init,
                           BinaryOperator op) {
    long no_threads = std::thread::hardware_concurrency();
    return parallel_accumulate(begin, end, init, op,
                               (no_threads > 1) ? no_threads : 2);
}

template <class Iterator, class T>
T parallel_accumulate_auto(Iterator begin, Iterator end, T init) {
    long no_threads = std::thread::hardware_concurrency();
    return parallel_accumulate(begin, end, std::plus<T>(), init,
                               (no_threads > 1) ? no_threads : 2);
}

template <class Iterator, class Functor>
void parallel_for_each(Iterator begin, Iterator end, Functor f,
                       std::size_t no_threads) {
    std::size_t length = std::distance(begin, end);
    std::size_t block_size = length / no_threads;
    std::vector<std::thread> threads;
    auto block_start = begin;
    for (std::size_t i = 0; i != (no_threads - 1); ++i) {
        auto block_end = block_start;
        std::advance(block_end, block_size);
        threads.emplace_back(detail::for_each<Iterator, Functor>(), block_start,
                             block_end, f);
        block_start = block_end;
    }
    threads.emplace_back(detail::for_each<Iterator, Functor>(), block_start,
                         end, f);
    for (auto& t : threads) {
        t.join();
    }
}

template <class Iterator, class Functor>
void parallel_for_each_auto(Iterator begin, Iterator end, Functor f) {
    long no_threads = std::thread::hardware_concurrency();
    parallel_for_each(begin, end, f, (no_threads > 1) ? no_threads : 2);
}

/**
 * Using std::async for the calculations
 */

namespace detail {
template <class Iterator, class BinaryOperator>
struct async_sum {
    using value_t = typename std::iterator_traits<Iterator>::value_type;
    value_t operator()(Iterator start, Iterator end, BinaryOperator op) {
        value_t ret = *start;
        ++start;
        return std::accumulate(start, end, ret, op);
    }
};
}

template <class Iterator, class T, class BinaryOperator>
T async_accumulate(Iterator begin, Iterator end, T init, BinaryOperator op) {
    std::size_t threads = std::thread::hardware_concurrency();
    if (threads < 2) {
        threads = 2;
    }
    std::size_t length = std::distance(begin, end);
    std::size_t chunk_size = length / threads;
    std::list<std::future<T>> tmp_val;
    auto iter_start = begin;
    for (std::size_t i = 0; i != threads - 1; ++i) {
        auto iter_end = iter_start;
        std::advance(iter_end, chunk_size);
        tmp_val.emplace_back(std::async(
            std::launch::async, detail::async_sum<Iterator, BinaryOperator>(),
            iter_start, iter_end, op));
    }
    for (auto& t : tmp_val) {
        t.wait();
        init = op(init, t.get());
    }
    return init;
}

template <class Iterator, class T>
T async_accumulate(Iterator begin, Iterator end, T init) {
    return async_accumulate(begin, end, init, std::plus<T>());
}

}  // end js
