#include "catch.hpp"
#include "js/tuple.hpp"
#include <array>
#include <iostream>
#include <type_traits>

struct TestTupleClass {
    int value;
    TestTupleClass(int v) : value(v) {}
};

template <class T>
void add_onehundred(T&) {}

template <>
void add_onehundred<int>(int& t) {
    t += 100;
}

template <>
void add_onehundred<TestTupleClass>(TestTupleClass& t) {
    t.value += 100;
}

template <class T>
std::string transform(const T&);

template <>
std::string transform<int>(const int&) {
    return "Integer";
}

template <>
std::string transform<TestTupleClass>(const TestTupleClass&) {
    return "TestTupleClass";
}

TEST_CASE("Tuple functions") {
    auto value_tuple = std::make_tuple(1, TestTupleClass(2));

    SECTION("js::for_each_tuple") {
        js::for_each_tuple(value_tuple, [](auto& x) { add_onehundred(x); });
        CHECK(std::get<0>(value_tuple) == 101);
        CHECK(std::get<1>(value_tuple).value == 102);
    }

    SECTION("js::apply_tuple") {
        auto t = js::apply_tuple(
            [](int x, TestTupleClass y) {
                return std::make_tuple(x + 100, y.value + 100);
            },
            value_tuple);
        //        CHECK(std::is_same<decltype(t), std::tuple<int,int>>::value ==
        //        1);
        CHECK(std::get<0>(t) == 101);
        CHECK(std::get<1>(t) == 102);
    }

    SECTION("js::tuple_from_array") {
        std::array<int, 3> arr = {10, 11, 12};
        auto tupl = js::tuple_from_array(arr);

        constexpr std::size_t tupl_size =
            std::tuple_size<decltype(tupl)>::value;
        CHECK(tupl_size == arr.size());
        CHECK(std::get<0>(tupl) == arr[0]);
        CHECK(std::get<1>(tupl) == arr[1]);
        CHECK(std::get<2>(tupl) == arr[2]);
    }

    SECTION("js::functor_tuple") {
        auto tupl = js::functor_tuple(
            value_tuple, [](const auto& x) { return transform(x); });
        constexpr std::size_t is_req_tuple =
            std::is_same<std::tuple<std::string, std::string>,
                         decltype(tupl)>::value;
        CHECK(is_req_tuple);
        CHECK(std::get<0>(tupl) == "Integer");
        CHECK(std::get<1>(tupl) == "TestTupleClass");
    }

    SECTION("js::construct_from_tuple") {
        struct TestClass {
            int _a;
            double _b;
            TestClass(int a, double b) : _a(a), _b(b) {}
            TestClass(const TestClass&) = default;
            TestClass(TestClass&&) = default;
            ~TestClass() = default;
            TestClass& operator=(const TestClass&) = default;
            TestClass& operator=(TestClass&&) = default;
        };
        int a = 42;
        double b = 3.14;
        auto tuple_to_construct = std::make_tuple(a, b);
        TestClass test = js::construct_from_tuple(tuple_to_construct);

        CHECK(test._a == a);
        CHECK(test._b == b);
    }
}
