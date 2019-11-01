// #include <algorithm>
// #include <functional>
// #include <numeric>
// #include <string>
#include <vector>


#include "../include/drift.h"
//#include "../include/algorithm.h"
#include "../../catch2/catch.hpp"

TEST_CASE("simple counter", "[gen]") {
    
    SECTION("T1") {
        std::vector<int> v;
        for (auto i : drift::generator([n = 0]() mutable { return n++; })) {
            if (i > 9)
                break;
            v.push_back(i);
        }
        REQUIRE(v == std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    }
}
