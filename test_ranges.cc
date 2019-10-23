#include <array>
#include <vector>
#include <iostream>

#include "include/ranges.h"

int main() {
   // #ifdef XXXXX
    auto z = drift::zip<int*>();
    int a = 0, b = 1, c = 2;
    auto zz = drift::zip_iterator(&a,&b,&c);
    std::cout << "before: ";
    std::cout << "\na: " << a;
    std::cout << "\nb: " << b;
    std::cout << "\nc: " << c;
    
    auto [aref, bref, cref] = *zz;
    ++aref;
    ++bref;
    ++cref;

    std::cout << "\nafter: ";
    std::cout << "\na: " << a;
    std::cout << "\nb: " << b;
    std::cout << "\nc: " << c;

    int arr1[] {0, 1, 2};
    int arr2[] {3, 4, 5};
    auto zzz = drift::zip_iterator(arr1, arr2);

    for (int i = 0; i < 3; ++i) {
        auto [za1, za2] = *zzz;
        std::cout << "\nza1, za2 = " << za1 << ", " << za2;
        ++zzz;
    }
    --zzz;
    {
        auto [za1, za2] = zzz[0];
        std::cout << "\nsubscript access: za1, za2 = " << za1 << ", " << za2;  
    }

 //   auto zbeg = drift::zip_iterator(arr1, arr2);
 //   auto zend = drift::zip_iterator(arr1 + 3, arr2 + 3);
    for(auto zi = drift::zip_iterator(arr1, arr2), zend = drift::zip_iterator(arr1 + 3, arr2 + 3); zi != zend; ++zi) {
        auto [za1, za2] = *zi;
        std::cout << "\nzi[za1, za2]: " << za1 << ", " << za2;
    }
    std::cout << "\n";

    std::array<int, 3> sarr1 = {0, 1, 2};
    const std::array<int, 3> sarr2 = {3, 4, 5};
    for(auto [s1, s2] : drift::zip(sarr1, sarr2)) {
        std::cout << "\n[s1, s2]: " << s1 << ", " << s2;
        s1 = 3;
    }

    std::cout << "\nsecond time:";
    for(auto [s1, s2] : drift::zip(sarr1, sarr2)) {
        std::cout << "\n[s1, s2]: " << s1 << ", " << s2;
    }
    std::cout << "\nsize: " << drift::zip(sarr1, sarr2).size();
    
    // {
    // std::vector x = {1, 2, 3, 4, 5, 6};
    // std::vector<int> x_sq;

    // for (auto z = drift::zip_iterator(begin(x), back_inserter(x_sq)); z != drift::zip_iterator(end(x), back_inserter(x_sq) ); ++z) {
    //     auto [xel, x_sq_el] = *z;
    //     x_sq_el = xel*xel;
    // }
    //     std::cout << "\ntesting back_inserter0:";
    //     for(auto [x1,x2] : drift::zip(x, x_sq)) {
    //         std::cout << "\nx1, x2: " << x1 << ", " << x2;
    //     }
    // }

    /* back_inserter test */
    {
    std::vector x = {1, 2, 3, 4, 5, 6};
    std::vector<int> x_sq;
    {
        auto z2 = drift::zip_iterator(back_inserter(x_sq));
        for (auto z1 = drift::zip_iterator(begin(x));
             z1 != drift::zip_iterator(end(x)); ++z1, ++z2) {
            auto [xel] = *z1;
            auto [x_sq_el] = *z2;

            x_sq_el = xel * xel;
            //std::get<0>(*z2) = xel * xel;
        }
        std::cout << "\ntesting back_inserter1:";
        for(auto [x1,x2] : drift::zip(x, x_sq)) {
            std::cout << "\nx1, x2: " << x1 << ", " << x2;
        }
    }
    }


    // for (auto z = drift::zip_iterator(begin(x), back_inserter(x_sq));; ++z) {
    //     auto [xel, x_sq_el] = *z;
    //     x_sq_el = xel*xel;
    // }

//#endif
    // auto v = std::vector{1, 3, 5, 7};
    // auto w = std::vector{2, 4, 6, 8};
    
    // for (int i = 0; i < size(v); ++i)
    //     std::cout <<"\nv[" << i <<"] = " << v[i];

    // for (auto x : v) {
    //     std::cout << "\nx: " << x;
    // }

    // for (auto [x, y] : drift::zip(v, w)) {
    //     std::cout << "\nx, y: " << x << "," << y ;
    // }

    /* vector<bool> test */

    auto vb1 = std::vector{true, false, false, true};
    auto vb2 = std::vector{false, true, true, false};
    for(auto [b1, b2] : drift::zip(vb1, vb2)) {
        std::cout << "\nb1, b2: " << b1 << b2;
    }

    std::cout << "\ntesting enumerate";
    auto ex = std::vector{"zebra", "dog", "caiman", "frog"};
    for(auto [i, v] : drift::enumerate(ex)) {
        std::cout << "\n" << i << ": " << v;
    }
    
    std::cout << "\ntesting enumerate<bool>";
    for(auto [i, b] : drift::enumerate(vb1)) {
        std::cout << "\n" << i << ": " << b;
    }

    std::cout << "\ntesting enumerate and zip together";
    for(auto [i, vb] : drift::enumerate(drift::zip(vb1, vb2))) {
        auto [b1, b2] = vb;
        std::cout << "\n" << i << ": " << b1 << b2;
    }

    return 0;
}