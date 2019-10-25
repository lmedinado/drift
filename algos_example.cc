#include <iostream>
#include <vector>


#include "include/algorithm.h"
#include "include/drift.h"

int main() {

    std::vector v{1, 2, 3, 4, 5, 6, 7};
    std::vector w{8, 9, 0, 1, 2, 3, 4};
    std::vector<int> z(7);

    drift::transform(v, w, z, [](auto a, auto b) { return a + b; });

    for (auto [a, b, c] : drift::zip(v, w, z)) {
        std::cout << a << " + " << b << " = " << c << "\n";
    }

    std::cout << "there are " << drift::count(v, 3) << " 3s in v.\n";
    std::cout << "there are " << drift::count_if(v, [](auto x) { return x % 2; })
              << " odd numbers in v.\n";

    return 0;
}