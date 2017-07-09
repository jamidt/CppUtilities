#include "catch.hpp"
#include "js/iterator.hpp"
#include <array>
#include <type_traits>
#include <vector>

TEST_CASE("IteratorTuple") {
    std::vector<int> vec{10, 11, 12};
    std::array<int, 3> arr{20, 21, 22};

    using IteratorVectorInt = typename std::vector<int>::iterator;
    using IteratorArrayInt = typename std::array<int, 3>::iterator;
    using IteratorTupleVectorArray =
        js::IteratorTuple<IteratorVectorInt, IteratorArrayInt>;

    IteratorTupleVectorArray iter_vec_arr(vec.begin(), arr.begin());
    auto vec_iter_inc = vec.begin();
    ++vec_iter_inc;
    auto arr_iter_inc = arr.begin();
    ++arr_iter_inc;
    IteratorTupleVectorArray iter_vec_arr_inc(vec_iter_inc, arr_iter_inc);

    SECTION("Boolean equal/not equal") {
        IteratorTupleVectorArray iter_vec_arr2(vec.begin(), arr.begin());
        CHECK(iter_vec_arr == iter_vec_arr2);
        CHECK(!(iter_vec_arr == iter_vec_arr_inc));
    }

    SECTION("makeIteratorTuple") {
        auto iter_tuple_begin = js::makeIteratorTuple(vec.begin(), arr.begin());
        bool issame = std::is_same<decltype(iter_tuple_begin),
                                   IteratorTupleVectorArray>::value;
        CHECK(issame);
        CHECK(iter_tuple_begin == iter_vec_arr);
    }

    SECTION("Increment/Decrement operator") {
        SECTION("Prefix increment") {
            CHECK_FALSE(iter_vec_arr == iter_vec_arr_inc);
            CHECK(++iter_vec_arr == iter_vec_arr_inc);
        }
        SECTION("Prefix decrement") {
            CHECK_FALSE(iter_vec_arr == iter_vec_arr_inc);
            CHECK(iter_vec_arr == --iter_vec_arr_inc);
        }
        SECTION("Postfix increment") {
            CHECK_FALSE(iter_vec_arr == iter_vec_arr_inc);
            iter_vec_arr++;
            CHECK(iter_vec_arr == iter_vec_arr_inc);
        }
        SECTION("Postfix decrement") {
            CHECK_FALSE(iter_vec_arr == iter_vec_arr_inc);
            iter_vec_arr_inc--;
            CHECK(iter_vec_arr == iter_vec_arr_inc);
        }
    }

    SECTION("Assignment") {
        auto iter_tuple_assignment = iter_vec_arr_inc;
        CHECK(iter_tuple_assignment == iter_vec_arr_inc);
    }

    SECTION("Dereferencing") {
        CHECK(10 == std::get<0>(*iter_vec_arr));
        CHECK(11 == std::get<0>(*iter_vec_arr_inc));
        CHECK(20 == std::get<1>(*iter_vec_arr));
        CHECK(21 == std::get<1>(*iter_vec_arr_inc));
        std::get<1>(*iter_vec_arr) = 99;
        CHECK(99 == std::get<1>(*iter_vec_arr));
    }
}

TEST_CASE("ZipBase") {
    using VectorInt = std::vector<int>;
    using ArrayInt = std::array<int, 3>;
    VectorInt vec{10, 11, 12};
    ArrayInt arr{20, 21, 22};
    js::ZipBase<VectorInt, ArrayInt> zip1(vec, arr);

    SECTION("Iterators") {
        SECTION("Begin") {
            auto iter_begin = zip1.begin();
            auto iter_manual_begin =
                js::makeIteratorTuple(vec.begin(), arr.begin());
            CHECK(iter_begin == iter_manual_begin);
        }
        SECTION("End") {
            auto iter_end = zip1.end();
            auto iter_manual_end = js::makeIteratorTuple(vec.end(), arr.end());
            CHECK(iter_end == iter_manual_end);
        }
    }

    SECTION("Loops") {
        SECTION("For loop with iterators") {
            SECTION("Read") {
                int counter = 0;
                for (auto iter = zip1.begin(); iter != zip1.end(); ++iter) {
                    CHECK(std::get<0>(*iter) == (10 + counter));
                    CHECK(std::get<1>(*iter) == (20 + counter));
                    ++counter;
                }
            }
            SECTION("Write") {
                int counter = 0;
                for (auto iter = zip1.begin(); iter != zip1.end(); ++iter) {
                    std::get<0>(*iter) = 30 + counter;
                    std::get<1>(*iter) = 40 + counter;
                    ++counter;
                }
                counter = 0;
                for (auto iter = zip1.begin(); iter != zip1.end(); ++iter) {
                    CHECK(std::get<0>(*iter) == (30 + counter));
                    CHECK(std::get<1>(*iter) == (40 + counter));
                    ++counter;
                }
            }
        }

        SECTION("Range base for loops") {
            int counter = 0;
            for (auto&& x : zip1) {
                CHECK(std::get<0>(x) == (10 + counter));
                CHECK(std::get<1>(x) == (20 + counter));
                ++counter;
            }
        }
        SECTION("Range base for loops") {
            int counter = 0;
            for (auto&& x : zip1) {
                std::get<0>(x) = 30 + counter;
                std::get<1>(x) = 40 + counter;
                ++counter;
            }
            counter = 0;
            for (auto&& x : zip1) {
                CHECK(std::get<0>(x) == (30 + counter));
                CHECK(std::get<1>(x) == (40 + counter));
                ++counter;
            }
        }
    }
}
