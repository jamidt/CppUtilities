#include "catch.hpp"
#include "js/type_traits.hpp"

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>

TEST_CASE("std extensions") {
    SECTION("Conjugation") {
        auto val_false = js::conjugation_v<js::bool_constant<true>,
                                           js::bool_constant<false>>;
        CHECK_FALSE(val_false);
        auto val_false2 = js::conjugation_v<js::bool_constant<false>,
                                            js::bool_constant<true>>;
        CHECK_FALSE(val_false2);
        auto val_false3 = js::conjugation_v<js::bool_constant<false>,
                                            js::bool_constant<false>>;
        CHECK_FALSE(val_false3);
        auto val_true =
            js::conjugation_v<js::bool_constant<true>, js::bool_constant<true>>;
        CHECK(val_true);
    }
    SECTION("Disjunction") {
        auto val_false = js::disjunction_v<js::bool_constant<false>,
                                           js::bool_constant<false>>;
        CHECK_FALSE(val_false);
        auto val_true = js::disjunction<js::bool_constant<true>,
                                        js::bool_constant<false>>::value;
        CHECK(val_true);
        auto val_true2 = js::disjunction_v<js::bool_constant<false>,
                                           js::bool_constant<true>>;
        CHECK(val_true2);
        auto val_true3 =
            js::disjunction_v<js::bool_constant<true>, js::bool_constant<true>>;
        CHECK(val_true3);
    }
    //    SECTION("Positive integral") {
    //        auto val_true = js::is_positive_integral_v<int, 3>;
    //        CHECK(val_true);
    //        auto val_false = js::is_positive_integral<double, 3.0>::value;
    //        CHECK_FALSE( val_false );
    //        auto val_false2 = js::is_positive_integral_v<int, -2>;
    //        CHECK_FALSE(val_false2);
    //    }
    SECTION("Negation") {
        auto val_false = js::negation<js::bool_constant<true>>::value;
        CHECK_FALSE(val_false);
        auto val_true = js::negation<js::bool_constant<false>>::value;
        CHECK(val_true);
    }
    SECTION("fulfills") {
        auto val_true =
            js::fulfills<int, std::is_signed, std::is_integral>::value;
        CHECK(val_true);
        auto val_false =
            js::fulfills<double, std::is_signed, std::is_integral>::value;
        CHECK_FALSE(val_false);
    }
}

TEST_CASE("ContainerTraits") {
    SECTION("IsSequenceContainer") {
        auto pdt1 = js::IsSequenceContainer<double>::value;
        CHECK_FALSE(pdt1);
        auto v1 = js::IsSequenceContainer<std::vector<int>>::value;
        CHECK(v1);
        auto a1 = js::IsSequenceContainer<std::array<int, 3>>::value;
        CHECK(a1);
        auto l1 = js::IsSequenceContainer<std::list<int>>::value;
        CHECK(l1);
        auto dq1 = js::IsSequenceContainer<std::deque<int>>::value;
        CHECK(dq1);
        auto fwl1 = js::IsSequenceContainer<std::forward_list<int>>::value;
        CHECK(fwl1);
    }
    SECTION("HasRandomAccess") {
        auto pdt1 = js::HasRandomAccess<int>::value;
        CHECK_FALSE(pdt1);
        auto v1 = js::HasRandomAccess<std::vector<int>>::value;
        CHECK(v1);
        auto a1 = js::HasRandomAccess<std::array<int, 2>>::value;
        CHECK(a1);
        auto l1 = js::HasRandomAccess<std::list<int>>::value;
        CHECK_FALSE(l1);
    }
}
