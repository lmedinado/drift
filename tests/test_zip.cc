#include <array>
#include <list>
#include <forward_list>
#include <type_traits>
#include <vector>

#include "../include/ranges.h"
#include "../../catch2/catch.hpp"


TEST_CASE("zip ranges can be constructed", "[zip]") {
    SECTION("empty zip ranges are well-defined") { auto z = lranges::zip<>(); }
    SECTION("zip ranges are default constructible") { auto z = lranges::zip<int *>(); }
}

TEST_CASE("can access by a zip iterator", "[zip]") {

    SECTION("three pointers to int") {
        int a = 0, b = 1, c = 2;
        auto zz = lranges::zip_iterator(&a, &b, &c);

        auto [aref, bref, cref] = *zz;
        ++aref;
        ++bref;
        ++cref;
        REQUIRE(a == 1);
        REQUIRE(b == 2);
        REQUIRE(c == 3);
    }

    int arr1[]{0, 1, 2};
    int arr2[]{3, 4, 5};
    auto zzz = lranges::zip_iterator(arr1, arr2);

    SECTION("incrementing a zip iterator") {
        for (int i = 0; i < 3; ++i) {
            auto [za1, za2] = *zzz;
            REQUIRE(za1 == arr1[i]);
            REQUIRE(za2 == arr2[i]);
            ++zzz;
        }
    }

    SECTION("subscript operator access") {
        auto [za1, za2] = zzz[2];
        REQUIRE(za1 == 2);
        REQUIRE(za2 == 5);
        // std::cout << "\nsubscript access: za1, za2 = " << za1 << ", " << za2;
    }

    SECTION("for loop with manually defined bounds") {
        int i = 0;
        using std::size;
        for (auto zi = lranges::zip_iterator(arr1, arr2),
                  zend = lranges::zip_iterator(arr1 + size(arr1), arr2 + size(arr2));
             zi != zend; ++zi) {
            auto [za1, za2] = *zi;
            REQUIRE(za1 == arr1[i]);
            REQUIRE(za2 == arr2[i]);
            ++i;
            // std::cout << "\nzi[za1, za2]: " << za1 << ", " << za2;
        }
    }

    SECTION("post-increment") {
        int i = 0;
        using std::size;
        for (auto zi = lranges::zip_iterator(arr1, arr2),
                  zend = lranges::zip_iterator(arr1 + size(arr1), arr2 + size(arr2));
             zi != zend; zi++) {
            auto [za1, za2] = *zi;
            REQUIRE(za1 == arr1[i]);
            REQUIRE(za2 == arr2[i]);
            ++i;
            // std::cout << "\nzi[za1, za2]: " << za1 << ", " << za2;
        }
    }

    SECTION("reverse iterators") {
        using std::rbegin;
        using std::rend;
        using std::size;
        int i = size(arr1) - 1;
        for (auto zi = lranges::zip_iterator(rbegin(arr1), rbegin(arr2)),
                  zend = lranges::zip_iterator(rend(arr1), rend(arr2));
             zi != zend; ++zi) {
            auto [za1, za2] = *zi;
            REQUIRE(za1 == arr1[i]);
            REQUIRE(za2 == arr2[i]);
            --i;
        }
    }

    SECTION("pre-decrement") {
        using std::begin;
        using std::end;
        using std::size;
        int i = size(arr1) - 1;
        for (auto zi = lranges::zip_iterator(begin(arr1) + size(arr1) - 1, begin(arr2) + size(arr2) - 1),
                  zend = lranges::zip_iterator(begin(arr1) - 1, begin(arr2) - 1);
             zi != zend; --zi) {
            auto [za1, za2] = *zi;
            REQUIRE(za1 == arr1[i]);
            REQUIRE(za2 == arr2[i]);
            --i;
        }
    }

    SECTION("post-decrement") {
        using std::begin;
        using std::end;
        using std::size;
        int i = size(arr1) - 1;
        for (auto zi = lranges::zip_iterator(begin(arr1) + size(arr1) - 1, begin(arr2) + size(arr2) - 1),
                  zend = lranges::zip_iterator(begin(arr1) - 1, begin(arr2) - 1);
             zi != zend; zi--) {
            auto [za1, za2] = *zi;
            REQUIRE(za1 == arr1[i]);
            REQUIRE(za2 == arr2[i]);
            --i;
        }
    }

    std::array<int, 3> sarr1 = {0, 1, 2};
    const std::array<int, 3> sarr2 = {3, 4, 5};

    SECTION("size member function") { REQUIRE(lranges::zip(sarr1, sarr2).size() == 3); }

    SECTION("range-based for, with std::array") {
        int i = 0;
        for (auto [s1, s2] : lranges::zip(sarr1, sarr2)) {
            REQUIRE(s1 == sarr1[i]);
            REQUIRE(s2 == sarr2[i]);
            ++i;
            // std::cout << "\n[s1, s2]: " << s1 << ", " << s2;
        }
    }

    SECTION("range-based for, with mutation") {
        int i = 0;
        for (auto [s1, s2] : lranges::zip(sarr1, sarr2)) {
            REQUIRE(s1 == sarr1[i]);
            REQUIRE(s2 == sarr2[i]);
            // std::cout << "\n[s1, s2]: " << s1 << ", " << s2;
            s1 = 3;
            ++i;
        }

        i = 0;
        for (auto [s1, s2] : lranges::zip(sarr1, sarr2)) {
            REQUIRE(s1 == 3);
            REQUIRE(s2 == sarr2[i]);
            // std::cout << "\n[s1, s2]: " << s1 << ", " << s2;
            s1 = 3;
            ++i;
        }
    }

    std::vector x = {1, 2, 3, 4, 5, 6};
    std::vector<int> x_sq;
    SECTION("back_inserter test") {
        auto z2 = lranges::zip_iterator(back_inserter(x_sq));
        for (auto z1 = lranges::zip_iterator(begin(x));
             z1 != lranges::zip_iterator(end(x)); ++z1, ++z2) {
            auto [xel] = *z1;
            auto [x_sq_el] = *z2;

            x_sq_el = xel * xel;
        }

        for (auto [x1, x2] : lranges::zip(x, x_sq)) {
            REQUIRE(x2 == x1 * x1);
        }
    }

    auto vb1 = std::vector{true, false, false, true};
    auto vb2 = std::vector{false, true, true, false};
    SECTION("proxy iterator access") {
        int i = 0;
        for (auto [b1, b2] : lranges::zip(vb1, vb2)) {
            REQUIRE(b1 == vb1[i]);
            REQUIRE(b2 == vb2[i]);
            // std::cout << "\nb1, b2: " << b1 << b2;
            ++i;
        }
    }

    SECTION("zip together with enumerate") {
        // std::cout << "\ntesting enumerate and zip together";
        for (auto [i, vb] : lranges::enumerate(lranges::zip(vb1, vb2))) {
            auto [b1, b2] = vb;
            REQUIRE(b1 == vb1[i]);
            REQUIRE(b2 == vb2[i]);
            // std::cout << "\n" << i << ": " << b1 << b2;
        }
    }
}

TEST_CASE("correctly identifies iterator category", "[zip]") {
    SECTION("for pointers") {

        auto z = lranges::zip<int *>();
        REQUIRE(std::is_same_v<decltype(z.begin())::iterator_category, std::random_access_iterator_tag>);

        int a = 0, b = 1, c = 2;
        auto zz = lranges::zip_iterator(&a, &b, &c);

        REQUIRE(std::is_same_v<decltype(zz)::iterator_category, std::random_access_iterator_tag>);
    }
    SECTION("for C-style arrays") {
        int arr1[]{0, 1, 2};
        int arr2[]{3, 4, 5};
        auto zzz = lranges::zip_iterator(arr1, arr2);
        REQUIRE(std::is_same_v<decltype(zzz)::iterator_category, std::random_access_iterator_tag>);
    }

    SECTION("for std::arrays") {
        std::array<int, 3> sarr1 = {0, 1, 2};
        const std::array<int, 3> sarr2 = {3, 4, 5};
        auto zzz = lranges::zip(sarr1, sarr2);
        REQUIRE(std::is_same_v<decltype(zzz.begin())::iterator_category, std::random_access_iterator_tag>);
    }

    SECTION("for std::lists") {
        const std::list<int> sl1 = {0, 1, 2};
        const std::list<int> sl2 = {3, 4, 5};
        auto zl = lranges::zip(sl1, sl2);
        REQUIRE(std::is_same_v<decltype(zl.begin())::iterator_category, std::bidirectional_iterator_tag>);
    }

    SECTION("for std::forward_lists") {
        const std::forward_list<int> sl1 = {0, 1, 2};
        const std::forward_list<int> sl2 = {3, 4, 5};
        auto zl = lranges::zip(sl1, sl2);
        REQUIRE(std::is_same_v<decltype(zl.begin())::iterator_category, std::forward_iterator_tag>);
    }

    SECTION("for vector<bool>") {
        auto vb1 = std::vector{true, false, false, true};
        auto vb2 = std::vector{false, true, true, false};
        auto zr = lranges::zip(vb1, vb2);
        REQUIRE(std::is_same_v<decltype(zr.begin())::iterator_category, std::random_access_iterator_tag>);
    }

    SECTION("for back_inserter") {
        std::vector<int> x_sq;
        auto z2 = lranges::zip_iterator(back_inserter(x_sq));
        REQUIRE(std::is_same_v<decltype(z2)::iterator_category, std::output_iterator_tag>);
    }
}

TEST_CASE("zip range member functions") {

    auto v1 = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto v2 = std::vector{1., 2., 3., 4., 5., 6., 7., 8., 9., 10.};

    SECTION("begin iterator") {
        REQUIRE(lranges::zip(v1, v2).begin() == lranges::zip_iterator(begin(v1), begin(v2)));
    }

    SECTION("begin iterator, found by adl") {
        using std::begin;
        REQUIRE(begin(lranges::zip(v1, v2)) == lranges::zip_iterator(begin(v1), begin(v2)));
    }


    SECTION("end iterator") {
        REQUIRE(lranges::zip(v1, v2).end() == lranges::zip_iterator(end(v1), end(v2)));
    }

    SECTION("end iterator, found by adl") {
        using std::end;
        REQUIRE(end(lranges::zip(v1, v2)) == lranges::zip_iterator(end(v1), end(v2)));
    }

    SECTION("size") { REQUIRE(lranges::zip(v1, v2).size() == size(v1)); }

    SECTION("size, found by adl") {
        using std::size;
        REQUIRE(size(lranges::zip(v1, v2)) == size(v1));
    }
}

TEST_CASE("random access zip iterator operations", "[zip]") {

    auto v1 = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto v2 = std::vector{1., 2., 3., 4., 5., 6., 7., 8., 9., 10.};
    SECTION("arithmetic operations -- plus equals") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin(); vb != lranges::zip(v1, v2).end(); vb += 2) {
            auto [vii, vif] = *vb;
            REQUIRE(vii == v1[i]);
            REQUIRE(vif == v2[i]);
            i += 2;
        }
    }

    SECTION("arithmetic operations -- non-member plus") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin(); vb != lranges::zip(v1, v2).end();
             vb = vb + 2) {
            auto [vii, vif] = *vb;
            REQUIRE(vii == v1[i]);
            REQUIRE(vif == v2[i]);
            i += 2;
        }
    }

    SECTION("arithmetic operations -- non-member plus, integer first") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin(); vb != lranges::zip(v1, v2).end();
             vb = 2 + vb) {
            auto [vii, vif] = *vb;
            REQUIRE(vii == v1[i]);
            REQUIRE(vif == v2[i]);
            i += 2;
        }
    }

    SECTION("arithmetic operations -- nonmember plus, negative integer first") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin() + 1; vb != lranges::zip(v1, v2).end();
             vb = 2 + vb) {
            vb = -1 + vb;
            auto [vii, vif] = *vb;
            REQUIRE(vii == v1[i]);
            REQUIRE(vif == v2[i]);

            ++i;
        }
    }

    SECTION("arithmetic operations -- minus equals") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin() + 1; vb != lranges::zip(v1, v2).end();
             vb += 2) {
            vb -= 1;
            auto [vii, vif] = *vb;
            REQUIRE(vii == v1[i]);
            REQUIRE(vif == v2[i]);

            ++i;
        }
    }

    SECTION("arithmetic operations -- nonmember minus") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin() + 1; vb != lranges::zip(v1, v2).end();
             vb = vb + 2) {
            vb = vb - 1;
            auto [vii, vif] = *vb;
            REQUIRE(vii == v1[i]);
            REQUIRE(vif == v2[i]);

            ++i;
        }
    }

    SECTION("relational operators -- ==") {

        auto vb = lranges::zip(v1, v2).begin(), ve = lranges::zip(v1, v2).end();

        REQUIRE(vb == vb);
        REQUIRE(!(vb == vb + 1));
        REQUIRE(!(vb + 1 == vb));
        REQUIRE(!(vb == ve));
    }

    SECTION("relational operators -- !=") {

        auto vb = lranges::zip(v1, v2).begin(), ve = lranges::zip(v1, v2).end();

        REQUIRE(!(vb != vb));
        REQUIRE((vb != vb + 1));
        REQUIRE((vb + 1 != vb));
        REQUIRE((vb != ve));
    }


    SECTION("relational operators -- < loop") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin(); vb < lranges::zip(v1, v2).end(); ++vb) {
            auto [vii, vif] = *vb;
            REQUIRE(vii == v1[i]);
            REQUIRE(vif == v2[i]);
            ++i;
        }
        REQUIRE(i == size(v1));
    }

    SECTION("relational operators -- <") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin(), ve = lranges::zip(v1, v2).end();
             vb != ve; ++vb) {
            auto [vii, vif] = *vb;
            REQUIRE(vb < ve);
            REQUIRE(!(ve < vb));
            ++i;
        }
    }

    SECTION("relational operators -- <=") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin(), ve = lranges::zip(v1, v2).end();
             vb != ve; ++vb) {
            auto [vii, vif] = *vb;
            REQUIRE(vb <= ve);
            REQUIRE(vb <= vb);
            REQUIRE(vb <= vb + 1);
            REQUIRE(!(vb + 1 <= vb));
            REQUIRE(!(ve <= vb));

            ++i;
        }
    }

    SECTION("relational operators -- >") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin(), ve = lranges::zip(v1, v2).end();
             vb != ve; ++vb) {
            auto [vii, vif] = *vb;
            REQUIRE(ve > vb);
            REQUIRE(!(vb > ve));
            ++i;
        }
    }

    SECTION("relational operators -- >=") {
        int i = 0;
        for (auto vb = lranges::zip(v1, v2).begin(), ve = lranges::zip(v1, v2).end();
             vb != ve; ++vb) {
            auto [vii, vif] = *vb;
            REQUIRE(ve >= vb);
            REQUIRE(vb >= vb);
            REQUIRE(vb + 1 >= vb);
            REQUIRE(!(vb >= vb + 1));
            REQUIRE(!(vb >= ve));
            ++i;
        }
    }
}