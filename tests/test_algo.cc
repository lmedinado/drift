#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
#include <vector>


#include "../include/drift.h"
#include "../include/algorithm.h"
#include "../../catch2/catch.hpp"

TEST_CASE("all_of, any_of, none_of", "[algo]") {

    std::vector<int> v(10, 2);
    std::partial_sum(v.cbegin(), v.cend(), v.begin());

    SECTION("all_of") {
        REQUIRE(drift::all_of(v, [](int i) { return i % 2 == 0; }));
    }
    SECTION("none_of") {
        REQUIRE(drift::none_of(v, std::bind(std::modulus<int>(), std::placeholders::_1, 2)));
    }
    SECTION("any_of") {
        struct DivisibleBy {
            const int d;
            DivisibleBy(int n) : d(n) {}
            bool operator()(int n) const { return n % d == 0; }
        };

        REQUIRE(drift::any_of(v, DivisibleBy(7)));
    }
}
TEST_CASE("for_each", "[algo]") {

    struct Sum {
        Sum() : sum{0} {}
        void operator()(int n) { sum += n; }
        int sum;
    };
    std::vector<int> nums{3, 4, 2, 8, 15, 267};
    drift::for_each(nums, [](int &n) { n++; });
    SECTION("for_each (el)") { REQUIRE(nums == std::vector<int>{4, 5, 3, 9, 16, 268}); }
    SECTION("for_each (sum)") {
        // calls Sum::operator() for each number
        Sum s = drift::for_each(nums, Sum());

        REQUIRE(s.sum == 305);
    }
}

TEST_CASE("count", "[algo]") {
    std::vector<int> v{1, 2, 3, 4, 4, 3, 7, 8, 9, 10};

    SECTION("target value") {
        // determine how many integers in a std::vector match a target value.
        int target1 = 3;
        int target2 = 5;
        int num_items1 = drift::count(v, target1);
        int num_items2 = drift::count(v, target2);
        REQUIRE(num_items1 == 2);
        REQUIRE(num_items2 == 0);
    }
    SECTION("conditional") {
        // use a lambda expression to count elements divisible by 3.
        int num_items3 = drift::count_if(v, [](int i) { return i % 3 == 0; });
        REQUIRE(num_items3 == 3);
    }
}

TEST_CASE("mismatch", "[algo]") {
    static auto mirror_ends = [](const std::string &in) {
        return std::string(in.begin(), drift::mismatch(in, drift::reverse_view(in)).first);
    };
    SECTION("mirroring") {
        REQUIRE(mirror_ends("abXYZba") == "ab");
        REQUIRE(mirror_ends("abca") == "a");
        REQUIRE(mirror_ends("aba") == "aba");
    }
}

TEST_CASE("find", "[algo]") {
    int n1 = 3;
    int n2 = 5;

    std::vector<int> v{0, 1, 2, 3, 4};

    SECTION("simple find") {
        auto result1 = drift::find(v, n1);
        auto result2 = drift::find(v, n2);

        REQUIRE(result1 != std::end(v));
        REQUIRE(*result1 == n1);

        REQUIRE(result2 == std::end(v));
    }
}

TEST_CASE("adjacent find", "[algo]") {
    std::vector<int> v1{0, 1, 2, 3, 40, 40, 41, 41, 5};
    SECTION("T1") {
        auto i1 = drift::adjacent_find(v1);

        REQUIRE(i1 != v1.end());
        REQUIRE(std::distance(v1.begin(), i1) == 4);
    }

    SECTION("T2") {
        auto i2 = drift::adjacent_find(v1, std::greater<int>());
        REQUIRE(i2 != v1.end());
        REQUIRE(std::distance(v1.begin(), i2) == 7);
    }
}

TEST_CASE("copy") {

    std::vector<int> from_vector(10);
    std::iota(from_vector.begin(), from_vector.end(), 0);

    // SECTION("copy with back_inserter") {
    //     std::vector<int> to_vector;
    //     drift::copy(from_vector, std::back_inserter(to_vector));

    //     REQUIRE(from_vector == to_vector);
    // }

    SECTION("copy with prealloc") {
        std::vector<int> to_vector(from_vector.size());
        drift::copy(from_vector, to_vector);

        REQUIRE(from_vector == to_vector);
    }
}

TEST_CASE("move") {
    std::vector<std::unique_ptr<int>> v;
    v.emplace_back(new int(1));
    v.emplace_back(new int(2));
    v.emplace_back(new int(3));
    std::vector<std::unique_ptr<int>> l(size(v));
    // copy() would not compile, because std::unique_ptr is noncopyable

    drift::move(v, l);
    REQUIRE(*(l[0]) == 1);
    REQUIRE(*(l[1]) == 2);
    REQUIRE(*(l[2]) == 3);
}

TEST_CASE("fill") {

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    SECTION("T1") {
        drift::fill(v, -1);

        for (auto elem : v)
            REQUIRE(elem == -1);
    }
}

TEST_CASE("transform") {
    std::string s("hello");
    SECTION("same type") {
        drift::transform(s, s,
                         [](unsigned char c) -> unsigned char { return std::toupper(c); });
        REQUIRE(s == "HELLO");
    }
    SECTION("different types") {
        drift::transform(s, s,
                         [](unsigned char c) -> unsigned char { return std::toupper(c); });
        std::vector<std::size_t> ordinals(size(s));
        drift::transform(s, ordinals, [](unsigned char c) -> std::size_t { return c; });
        REQUIRE(ordinals == std::vector<size_t>{72, 69, 76, 76, 79});
    }
}

TEST_CASE("generate") {
    std::vector<int> v(5);

    SECTION("constant") {
        drift::generate(v, []() { return 4; });

        for (auto iv : v) {
            REQUIRE(iv == 4);
        }
    }
    SECTION("mutable lambda") {
        // Initialize with default values 0,1,2,3,4 from a lambda function
        // Equivalent to std::iota(v.begin(), v.end(), 0);
        drift::generate(v, [n = 0]() mutable { return n++; });

        REQUIRE(v == std::vector<int>{0, 1, 2, 3, 4});
    }
}

/* minimum/maximum operations */
TEST_CASE("max_element") {

    std::vector<int> v{3, 1, -14, 1, 5, 9};

    SECTION("normal") {
        auto result = drift::max_element(v);
        REQUIRE(std::distance(v.begin(), result) == 5);
        REQUIRE(*result == 9);
    }
    SECTION("with functor") {
        auto abs_compare = [](int a, int b) { return (std::abs(a) < std::abs(b)); };
        auto result = drift::max_element(v, abs_compare);
        REQUIRE(std::distance(v.begin(), result) == 2);
        REQUIRE(*result == -14);
    }
}

TEST_CASE("min_element") {
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    SECTION("normal") {

        std::vector<int>::iterator result = drift::min_element(v);
        REQUIRE(std::distance(v.begin(), result) == 1);
        REQUIRE(*result == 1);
    }
}

TEST_CASE("minmax_element") {
    const auto v = {3, 9, 1, 4, 2, 5, 9};
    SECTION("T1") {
        const auto [min, max] = drift::minmax_element(v);

        REQUIRE(*min == 1);
        REQUIRE(*max == 9);
    }
}

/* numeric operations */
TEST_CASE("accumulate") {

    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SECTION("sum and product") {
        int sum = drift::accumulate(v, 0);
        REQUIRE(sum == 55);

        int product = drift::accumulate(v, 1, std::multiplies<int>());
        REQUIRE(product == 3628800);
    }

    SECTION("with strings") {
        auto dash_fold = [](std::string a, int b) {
            return std::move(a) + '-' + std::to_string(b);
        };

        std::string s = drift::accumulate(drift::tail(v),
                                          std::to_string(v.front()), // start with first element
                                          dash_fold);

        // Right fold using reverse iterators
        std::string rs = drift::accumulate(drift::tail(drift::reverse_view(v)),
                                           std::to_string(v.back()), // start with last element
                                           dash_fold);

        REQUIRE(s == "1-2-3-4-5-6-7-8-9-10");
        REQUIRE(rs == "10-9-8-7-6-5-4-3-2-1");
    }
}

TEST_CASE("inner_product") {
    std::vector<int> a{0, 1, 2, 3, 4};
    std::vector<int> b{5, 4, 2, 3, 1};

    SECTION("normal") {
        int r1 = drift::inner_product(a, b, 0);
        REQUIRE(r1 == 21);
    }

    SECTION("with functor") {
        int r2 = drift::inner_product(a, b, 0, std::plus<>(), std::equal_to<>());
        REQUIRE(r2 == 2);
    }
}