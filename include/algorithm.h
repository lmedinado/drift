#pragma once
#include <algorithm>

namespace drift {

#define DRIFT_ONE_IN(algo)                                \
    template <typename InRange>                           \
    constexpr auto algo(InRange &&in_range) {             \
        using std::begin;                                 \
        using std::end;                                   \
                                                          \
        return std::algo(begin(in_range), end(in_range)); \
    }

#define DRIFT_ONE_IN_ONE_T(algo)                                              \
    template <typename InRange, typename T>                                   \
    constexpr auto algo(InRange &&in_range, T &&t) {                          \
        using std::begin;                                                     \
        using std::end;                                                       \
                                                                              \
        return std::algo(begin(in_range), end(in_range), std::forward<T>(t)); \
    }

#define DRIFT_ONE_IN_TWO_T(algo)                                               \
    template <typename InRange, typename T1, typename T2>                      \
    constexpr auto algo(InRange &&in_range, T1 &&t1, T2 &&t2) {                \
        using std::begin;                                                      \
        using std::end;                                                        \
                                                                               \
        return std::algo(begin(in_range), end(in_range), std::forward<T1>(t1), \
                         std::forward<T2>(t2));                                \
    }

#define DRIFT_ONE_IN_ONE_OUT(algo)                                          \
    template <typename InRange, typename OutRange>                          \
    constexpr auto algo(InRange &&in_range, OutRange &&out_range) {         \
        using std::begin;                                                   \
        using std::end;                                                     \
                                                                            \
        return std::algo(begin(in_range), end(in_range), begin(out_range)); \
    }

#define DRIFT_ONE_IN_ONE_OUT_ONE_T(algo)                                   \
    template <typename InRange, typename OutRange, typename T>             \
    constexpr auto algo(InRange &&in_range, OutRange &&out_range, T &&t) { \
        using std::begin;                                                  \
        using std::end;                                                    \
                                                                           \
        return std::algo(begin(in_range), end(in_range), begin(out_range), \
                         std::forward<T>(t));                              \
    }

#define DRIFT_ONE_IN_ONE_OUT_TWO_T(algo)                                              \
    template <typename InRange, typename OutRange, typename T1, typename T2>          \
    constexpr auto algo(InRange &&in_range, OutRange &&out_range, T1 &&t1, T2 &&t2) { \
        using std::begin;                                                             \
        using std::end;                                                               \
                                                                                      \
        return std::algo(begin(in_range), end(in_range), begin(out_range),            \
                         std::forward<T1>(t1), std::forward<T2>(t2));                 \
    }

#define DRIFT_TWO_IN(algo)                                                    \
    template <typename InRange1, typename InRange2>                           \
    constexpr auto algo(InRange1 &&in_range1, InRange2 &&in_range2) {         \
        using std::begin;                                                     \
        using std::end;                                                       \
                                                                              \
        return std::algo(begin(in_range1), end(in_range1), begin(in_range2)); \
    }

#define DRIFT_TWO_IN_ONE_OUT_ONE_T(algo)                                           \
    template <typename InRange1, typename InRange2, typename OutRange, typename T> \
    constexpr auto algo(InRange1 &&in_range1, InRange2 &&in_range2,                \
                        OutRange &&out_range, T &&t) {                             \
        using std::begin;                                                          \
        using std::end;                                                            \
                                                                                   \
        return std::algo(begin(in_range1), end(in_range1), begin(in_range2),       \
                         begin(out_range), std::forward<T>(t));                    \
    }

/* std::all_of, std::any_of, std::none_of*/
DRIFT_ONE_IN_ONE_T(all_of)
DRIFT_ONE_IN_ONE_T(any_of)
DRIFT_ONE_IN_ONE_T(none_of)

/* std::for_each, std::for_each_n */
DRIFT_ONE_IN_ONE_T(for_each)
// DRIFT_ONE_IN_TWO_T(for_each_n)

/* std::count, std::count_if */
DRIFT_ONE_IN_ONE_T(count)
DRIFT_ONE_IN_ONE_T(count_if)

/* std::mismatch */
DRIFT_TWO_IN(mismatch)

/* std::find, std::find_if, std::find_if_not */
DRIFT_ONE_IN_ONE_T(find)
DRIFT_ONE_IN_ONE_T(find_if)
DRIFT_ONE_IN_ONE_T(find_if_not)

/* find_end, find_first_of*/
/* std::adjacent_find */
DRIFT_ONE_IN(adjacent_find)
DRIFT_ONE_IN_ONE_T(adjacent_find)

/* std::search */
/* std::search_n */
DRIFT_ONE_IN_TWO_T(search_n)

/* std::copy, std::copy_if*/
DRIFT_ONE_IN_ONE_OUT(copy)
DRIFT_ONE_IN_ONE_OUT_ONE_T(copy_if)
/* std::copy_n, std::copy_backward  */

/* std::move */
DRIFT_ONE_IN_ONE_OUT(move)
/* std::move_backward */

/* std::fill */
DRIFT_ONE_IN_ONE_T(fill)
/* std::fill_n */

/* std::transform */
DRIFT_ONE_IN_ONE_OUT_ONE_T(transform)
DRIFT_TWO_IN_ONE_OUT_ONE_T(transform)

/* std::generate */
DRIFT_ONE_IN_ONE_T(generate)
/* std::generate_n */

/* std::remove, std::remove_if */
DRIFT_ONE_IN_ONE_T(remove)
DRIFT_ONE_IN_ONE_T(remove_if)

/* std::remove_copy, std::remove_copy_if */
DRIFT_ONE_IN_ONE_OUT_ONE_T(remove_copy)
DRIFT_ONE_IN_ONE_OUT_ONE_T(remove_copy_if)

/* std::replace, std::replace_if */
DRIFT_ONE_IN_TWO_T(replace)
DRIFT_ONE_IN_TWO_T(replace_if)

/* std::replace_copy, std::replace_copy_if */
DRIFT_ONE_IN_ONE_OUT_TWO_T(replace_copy)
DRIFT_ONE_IN_ONE_OUT_TWO_T(replace_copy_if)

/* std::swap_ranges */
DRIFT_ONE_IN_ONE_OUT(swap_ranges)
/* std::iter_swap */

/* std::reverse */
DRIFT_ONE_IN(reverse)

/* std::reverse_copy */
DRIFT_ONE_IN_ONE_OUT(reverse_copy)

/* std::rotate, std::rotate_copy */

/* std::shuffle */
DRIFT_ONE_IN_ONE_T(shuffle)

/* std::sample */
DRIFT_ONE_IN_ONE_OUT_TWO_T(sample)

/* std::unique */
DRIFT_ONE_IN(unique)

/* std::unique_copy */
DRIFT_ONE_IN_ONE_OUT(unique_copy)

#undef DRIFT_ONE_IN
#undef DRIFT_ONE_IN_ONE_T
#undef DRIFT_ONE_IN_TWO_T
#undef DRIFT_ONE_IN_ONE_OUT
#undef DRIFT_ONE_IN_ONE_OUT_ONE_T
#undef DRIFT_ONE_IN_ONE_OUT_TWO_T

#undef DRIFT_TWO_IN
#undef DRIFT_TWO_IN_ONE_OUT_ONE_T
} // namespace drift