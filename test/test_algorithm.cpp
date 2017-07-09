#include "catch.hpp"
#include "js/algorithm.hpp"

TEST_CASE("Sort") {
    std::vector<int> test = {3, 6, 4, 1};
    SECTION("index_sort") {
        std::vector<std::size_t> expected = {3, 0, 2, 1};
        auto sorted_vec = js::index_sort(test.begin(), test.end());
        CHECK(sorted_vec == expected);
    }
}
