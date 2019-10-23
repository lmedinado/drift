#pragma once
#include <algorithm>

namespace drift {

#define DRIFT_ONE_IN_ONE_OUT(algo)                                          \
    template <typename InRange, typename OutRange>                          \
    auto algo(InRange &&in_range, OutRange &&out_range) {                   \
        using std::begin;                                                   \
        using std::end;                                                     \
                                                                            \
        return std::algo(begin(in_range), end(in_range), begin(out_range)); \
    }

#define DRIFT_ONE_IN_ONE_OUT_OP(algo)                                      \
    template <typename InRange, typename OutRange, typename UnaryOp>       \
    auto algo(InRange &&in_range, OutRange &&out_range, UnaryOp &&op) {    \
        using std::begin;                                                  \
        using std::end;                                                    \
                                                                           \
        return std::algo(begin(in_range), end(in_range), begin(out_range), \
                         std::forward<UnaryOp>(op));                       \
    }

#define DRIFT_TWO_IN_ONE_OUT_OP(algo)                                                     \
    template <typename InRange1, typename InRange2, typename OutRange, typename BinaryOp> \
    auto algo(InRange1 &&in_range1, InRange1 &&in_range2, OutRange &&out_range,           \
              BinaryOp &&op) {                                                            \
        using std::begin;                                                                 \
        using std::end;                                                                   \
                                                                                          \
        return std::algo(begin(in_range1), end(in_range1), begin(in_range2),              \
                         begin(out_range), std::forward<BinaryOp>(op));                   \
    }

DRIFT_ONE_IN_ONE_OUT(copy)
DRIFT_ONE_IN_ONE_OUT(move)
DRIFT_ONE_IN_ONE_OUT_OP(transform)
DRIFT_TWO_IN_ONE_OUT_OP(transform)
// template <typename InRange, typename OutRange>
// auto copy(InRange &&in_range, OutRange &&out_range) {
//     using std::begin;
//     using std::end;

//     return std::copy(begin(in_range), end(in_range), begin(out_range));
// }

// template <typename InRange, typename OutRange, typename UnaryOp>
// auto transform(InRange &&in_range, OutRange &&out_range, UnaryOp &&op) {
//     using std::begin;
//     using std::end;

//     return std::transform(begin(in_range), end(in_range), begin(out_range),
//                           std::forward<UnaryOp>(op));
// }

// template <typename InRange1, typename InRange2, typename OutRange, typename BinaryOp>
// auto transform(InRange1 &&in_range1, InRange1 &&in_range2, OutRange &&out_range, BinaryOp &&op) {
//     using std::begin;
//     using std::end;

//     return std::transform(begin(in_range1), end(in_range1), begin(in_range2),
//                           begin(out_range), std::forward<BinaryOp>(op));
// }

#undef DRIFT_ONE_IN_ONE_OUT
#undef DRIFT_ONE_IN_ONE_OUT_OP
#undef DRIFT_TWO_IN_ONE_OUT_OP
} // namespace drift