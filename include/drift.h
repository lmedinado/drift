/* drift is poor man's ranges.
 *
 * Copyright (c) 2019 - present, Leandro Medina de Oliveira
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * --- Optional exception to the license ---
 *
 * As an exception, if, as a result of your compiling your source code, portions
 * of this Software are embedded into a machine-executable object form of such
 * source code, you may redistribute such embedded portions in such object form
 * without including the above copyright and permission notices.
 */

#pragma once

#include <iterator>
#include <tuple>

namespace driftmeta {
/* Scott Meyers' TD trick */
template <typename T>
struct TD;

// std::cout << __PRETTY_FUNCTION__ <<"\n";

template <class T>
struct unwrap_refwrapper {
    using type = T;
};

template <class T>
struct unwrap_refwrapper<std::reference_wrapper<T>> {
    using type = T &;
};

template <typename... Types>
auto safe_forward_as_tuple(Types &&... args) {
    return std::tuple<typename unwrap_refwrapper<Types>::type...>(std::forward<Types>(args)...);
}

namespace detail {
template <typename Action, typename Tuple, size_t... Is>
constexpr decltype(auto) tuple_visit_impl(Tuple &&t, Action &&a, std::index_sequence<Is...>) {
    return driftmeta::safe_forward_as_tuple(
        std::forward<Action>(a)(std::get<Is>(std::forward<Tuple>(t)))...);
}
} // namespace detail

template <typename Action, typename Tuple>
constexpr decltype(auto) tuple_visit(Tuple &&t, Action &&a) {
    return detail::tuple_visit_impl(
        std::forward<Tuple>(t), std::forward<Action>(a),
        std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>());
}

#define DRIFTMETA_ADL_WRAP(fun)                 \
    template <typename T>                       \
    constexpr decltype(auto) adl_##fun(T &&t) { \
        using std::fun;                         \
        return fun(std::forward<T>(t));         \
    }

DRIFTMETA_ADL_WRAP(begin)
DRIFTMETA_ADL_WRAP(end)
DRIFTMETA_ADL_WRAP(cbegin)
DRIFTMETA_ADL_WRAP(cend)
DRIFTMETA_ADL_WRAP(rbegin)
DRIFTMETA_ADL_WRAP(rend)
DRIFTMETA_ADL_WRAP(size)

#undef DRIFTMETA_ADL_WRAP

} // namespace driftmeta

namespace drift {
/* iterator type traits */
template <typename It>
using iterator_category_t = typename std::iterator_traits<It>::iterator_category;

template <typename It>
using iter_difference_t = typename std::iterator_traits<It>::difference_type;

template <typename It>
using iter_value_t = typename std::iterator_traits<It>::value_type;

template <typename It>
using iter_reference_t = typename std::iterator_traits<It>::reference;


namespace detail {

template <typename ItCat>
constexpr bool is_decrementable = std::is_base_of_v<std::bidirectional_iterator_tag, ItCat>;

template <typename ItCat>
constexpr bool is_random_access = std::is_base_of_v<std::random_access_iterator_tag, ItCat>;

template <typename... Its>
struct weakest_iterator_tag {
private:
    constexpr static auto tag_deducer() {
        if constexpr ((std::is_base_of_v<std::random_access_iterator_tag, iterator_category_t<Its>> and
                       ...))
            return std::random_access_iterator_tag();
        else if constexpr ((std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category_t<Its>> and
                            ...))
            return std::bidirectional_iterator_tag();
        else if constexpr ((std::is_base_of_v<std::forward_iterator_tag, iterator_category_t<Its>> and
                            ...))
            return std::forward_iterator_tag();
        else if constexpr ((std::is_base_of_v<std::input_iterator_tag, iterator_category_t<Its>> and
                            ...))
            return std::input_iterator_tag();
        else if constexpr ((std::is_base_of_v<std::output_iterator_tag, iterator_category_t<Its>> and
                            ...))
            return std::output_iterator_tag();
        else
            return;
    }

public:
    using type = decltype(weakest_iterator_tag::tag_deducer());
};

template <typename... Its>
using weakest_iterator_tag_t = typename weakest_iterator_tag<Its...>::type;

} // namespace detail

/* zip iterator */
template <typename... Its>
class zip_iterator {
public:
    /* usual iterator typedefs */
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<typename std::iterator_traits<Its>::value_type...>;
    using pointer = std::tuple<typename std::iterator_traits<Its>::pointer...>;
    using reference = std::tuple<typename std::iterator_traits<Its>::reference...>;

    using iterator_category = detail::weakest_iterator_tag_t<Its...>;
    // driftmeta::TD<iterator_category> a;

    /* construction */
    zip_iterator() = default;

    template <typename U = int, std::enable_if_t<sizeof...(Its) != 0, U> = 0>
    explicit zip_iterator(Its... its) : its(its...) {}

    /* dereference */
    reference operator*() const {
        return driftmeta::tuple_visit(its, [](auto &&tel) -> decltype(auto) {
            return (*std::forward<decltype(tel)>(tel));
        });
    }

    /* certain output iterators don't return 'reference' */
    decltype(auto) operator*() {
        return driftmeta::tuple_visit(its, [](auto &&tel) -> decltype(auto) {
            return (*std::forward<decltype(tel)>(tel));
        });
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    /* increment */
    zip_iterator &operator++() {
        driftmeta::tuple_visit(its,
                           [](auto &&tel) { return ++std::forward<decltype(tel)>(tel); });
        return *this;
    }

    zip_iterator operator++(int) {
        auto temp = *this;
        // driftmeta::tuple_visit(its,
        //                    [](auto &&tel) { return ++std::forward<decltype(tel)>(tel); });
        ++(*this);
        return temp;
    }

    /* decrement */
    template <typename U = int, std::enable_if_t<detail::is_decrementable<iterator_category>, U> = 0>
    zip_iterator &operator--() {
        driftmeta::tuple_visit(its,
                           [](auto &&tel) { return --std::forward<decltype(tel)>(tel); });
        return *this;
    }

    template <typename U = int, std::enable_if_t<detail::is_decrementable<iterator_category>, U> = 0>
    zip_iterator operator--(int) {
        auto temp = *this;
        // driftmeta::tuple_visit(its,
        //                    [](auto &&tel) { return --std::forward<decltype(tel)>(tel); });
        --(*this);
        return temp;
    }

    /* arithmetic */
    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    zip_iterator &operator+=(difference_type n) {
        driftmeta::tuple_visit(
            its, [n](auto &&tel) { return std::forward<decltype(tel)>(tel) += n; });
        return *this;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    zip_iterator &operator-=(difference_type n) {
        driftmeta::tuple_visit(
            its, [n](auto &&tel) { return std::forward<decltype(tel)>(tel) -= n; });
        return *this;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend zip_iterator operator+(zip_iterator lhs, difference_type n) {
        driftmeta::tuple_visit(
            lhs.its, [n](auto &&tel) { return std::forward<decltype(tel)>(tel) += n; });
        return lhs;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend zip_iterator operator+(difference_type n, zip_iterator rhs) {
        driftmeta::tuple_visit(
            rhs.its, [n](auto &&tel) { return std::forward<decltype(tel)>(tel) += n; });
        return rhs;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend zip_iterator operator-(zip_iterator lhs, difference_type n) {
        driftmeta::tuple_visit(
            lhs.its, [n](auto &&tel) { return std::forward<decltype(tel)>(tel) -= n; });
        return lhs;
    }

    // template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    // friend zip_iterator operator-(difference_type n, zip_iterator rhs) {
    //     driftmeta::tuple_visit(
    //         rhs.its, [n](auto &&tel) { return std::forward<decltype(tel)>(tel) -= n; });
    //     return rhs;
    // }

    /* iterator subtraction */
    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend difference_type operator-(const zip_iterator &lhs, const zip_iterator &rhs) {
        if constexpr (std::tuple_size_v<decltype(its)> >= 1)
            return std::get<0>(lhs.its) - std::get<0>(rhs.its);
        else
            return 0;
    }

    /* comparison */
    friend bool operator==(const zip_iterator &lhs, const zip_iterator &rhs) {
        return lhs.its == rhs.its;
    }
    friend bool operator!=(const zip_iterator &lhs, const zip_iterator &rhs) {
        return !(lhs == rhs);
    }

    /* random access comparisons */
    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend bool operator<(const zip_iterator &lhs, const zip_iterator &rhs) {
        return lhs.its < rhs.its;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend bool operator>(const zip_iterator &lhs, const zip_iterator &rhs) {
        return lhs.its > rhs.its;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend bool operator<=(const zip_iterator &lhs, const zip_iterator &rhs) {
        return lhs.its <= rhs.its;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend bool operator>=(const zip_iterator &lhs, const zip_iterator &rhs) {
        return lhs.its >= rhs.its;
    }

private:
    std::tuple<Its...> its;
};

template <typename... Its>
class zip {
public:
    zip() = default;

    template <typename... Ranges>
    explicit zip(Ranges &&... ranges)
      : begin_(driftmeta::adl_begin(ranges)...), end_(driftmeta::adl_end(ranges)...) {}

    /* todo: add cbegin, cend, rbegin, etc. */
    zip_iterator<Its...> begin() const { return begin_; }
    zip_iterator<Its...> end() const { return end_; }
    iter_difference_t<zip_iterator<Its...>> size() const { return end_ - begin_; }

private:
    zip_iterator<Its...> begin_;
    zip_iterator<Its...> end_;
};

template <typename... Ranges>
zip(Ranges &&... ranges)->zip<decltype(driftmeta::adl_begin(ranges))...>;

/* workaround for gcc bug 80438 https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80438 */
#ifdef __GNUC__
template <typename Range0, typename... Ranges>
zip(Range0 &&range0, Ranges &&... ranges) -> zip<decltype(driftmeta::adl_begin(range0)), decltype(driftmeta::adl_begin(ranges))...>;
#endif
/* indexed iterator */
template <typename It>
class indexed_iterator {
public:
    /* usual iterator typedefs */
    using difference_type = std::ptrdiff_t;
    using value_type =
        std::tuple<difference_type, typename std::iterator_traits<It>::value_type>;
    using pointer = std::tuple<difference_type, typename std::iterator_traits<It>::pointer>;
    using reference =
        std::tuple<difference_type, typename std::iterator_traits<It>::reference>;
    using iterator_category = typename std::iterator_traits<It>::iterator_category;

    indexed_iterator() = default;

    explicit indexed_iterator(It it) : it(it), begin_(it) {}
    explicit indexed_iterator(It it, It begin_) : it(it), begin_(begin_) {}

    /* dereference */
    reference operator*() const { return driftmeta::safe_forward_as_tuple(it - begin_, *it); }

    /* certain output iterators don't return 'reference' */
    // decltype(auto) operator*() { return std::make_pair(begin_ - it, *it); }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    /* increment */
    indexed_iterator &operator++() {
        ++it;
        return *this;
    }

    indexed_iterator &operator++(int) {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    template <typename U = int, std::enable_if_t<detail::is_decrementable<iterator_category>, U> = 0>
    indexed_iterator &operator--() {
        --it;
        return *this;
    }

    template <typename U = int, std::enable_if_t<detail::is_decrementable<iterator_category>, U> = 0>
    indexed_iterator operator--(int) {
        auto temp = *this;
        --(*this);
        return temp;
    }

    /* arithmetic */
    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    indexed_iterator &operator+=(difference_type n) {
        it += n;
        return *this;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    indexed_iterator &operator-=(difference_type n) {
        it -= n;
        return *this;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend indexed_iterator operator+(indexed_iterator lhs, difference_type n) {
        return lhs += n;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend indexed_iterator operator+(difference_type n, indexed_iterator rhs) {
        return rhs += n;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend indexed_iterator operator-(indexed_iterator lhs, difference_type n) {
        return lhs -= n;
    }

    /* iterator subtraction */
    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend difference_type operator-(const indexed_iterator &lhs, const indexed_iterator &rhs) {
        return lhs.it - rhs.it;
    }

    /* comparison */
    friend bool operator==(const indexed_iterator &lhs, const indexed_iterator &rhs) {
        return (lhs.it == rhs.it); // and (lhs.begin_ == rhs.begin_);
    }
    friend bool operator!=(const indexed_iterator &lhs, const indexed_iterator &rhs) {
        return !(lhs == rhs);
    }

    /* random access comparisons */
    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend bool operator<(const indexed_iterator &lhs, const indexed_iterator &rhs) {
        return lhs.it < rhs.it;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend bool operator>(const indexed_iterator &lhs, const indexed_iterator &rhs) {
        return lhs.it > rhs.it;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend bool operator<=(const indexed_iterator &lhs, const indexed_iterator &rhs) {
        return lhs.it <= rhs.it;
    }

    template <typename U = int, std::enable_if_t<detail::is_random_access<iterator_category>, U> = 0>
    friend bool operator>=(const indexed_iterator &lhs, const indexed_iterator &rhs) {
        return lhs.it >= rhs.it;
    }


private:
    It it, begin_;
};

template <typename It>
class indexed {
public:
    indexed() = default;

    template <typename Range>
    explicit indexed(Range &&range)
      : begin_(driftmeta::adl_begin(range)), end_(driftmeta::adl_end(range)) {}

    indexed_iterator<It> begin() const { return begin_; }
    indexed_iterator<It> end() const { return end_; }
    iter_difference_t<It> size() const { return end_ - begin_; }

private:
    indexed_iterator<It> begin_;
    indexed_iterator<It> end_;
};

template <typename Range>
indexed(Range &&range)->indexed<decltype(driftmeta::adl_begin(range))>;

/* generator */
template <typename Gen>
class generator_iterator {
public:
    /* usual iterator typedefs */
    using difference_type = std::ptrdiff_t;
    using value_type = std::invoke_result_t<Gen>;
    using pointer = value_type *;
    using reference = value_type;
    using iterator_category = std::input_iterator_tag;

    generator_iterator() = default;
    explicit generator_iterator(Gen gen)
      : gen(gen) //, result{gen()}
    {}

    /* dereference */
    // reference operator*() const { return result; }
    reference operator*() { return gen(); }

    /* increment */
    // generator_iterator &operator++() { result = gen(); }
    generator_iterator &operator++() { return *this; }

    generator_iterator &operator++(int) {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    /* comparison */
    friend bool operator==(const generator_iterator &lhs, const generator_iterator &rhs) {
        return (&lhs.gen == &rhs.gen); // and (lhs.result == rhs.result);
    }
    friend bool operator!=(const generator_iterator &lhs, const generator_iterator &rhs) {
        return !(lhs == rhs);
    }

private:
    Gen gen;
    // value_type result;
};

/* generating input iterator */
template <typename It>
class generator {
public:
    generator() = default;

    template <typename Gen>
    explicit generator( // typename generator_iterator<Gen>::value_type begin_,
                        // typename generator_iterator<Gen>::value_type end_,
        Gen &&gen)
      : begin_(gen) {}

    It begin() { return begin_; }
    It end() { return begin_; }

private:
    It begin_; // end_;
};

template <typename Gen>
generator(Gen &&)->generator<generator_iterator<Gen>>;

template <typename RIt>
class reverse_view {
public:
    reverse_view() = default;

    template <typename Range>
    explicit reverse_view(Range &&range)
      : begin_(std::make_reverse_iterator(driftmeta::adl_end(range))),
        end_(std::make_reverse_iterator(driftmeta::adl_begin(range))) {}

    RIt begin() const { return begin_; }
    RIt end() const { return end_; }
    iter_difference_t<RIt> size() const { return end_ - begin_; }

private:
    RIt begin_;
    RIt end_;
};

template <typename Range>
reverse_view(Range &&range)->reverse_view<decltype(driftmeta::adl_rbegin(range))>;

template <typename It>
class tail {
public:
    tail() = default;

    template <typename Range>
    explicit tail(Range &&range, size_t n = 1)
      : begin_(std::next(driftmeta::adl_begin(range), n)), end_(driftmeta::adl_end(range)) {}

    It begin() const { return begin_; }
    It end() const { return end_; }
    iter_difference_t<It> size() const { return end_ - begin_; }

private:
    It begin_;
    It end_;
};

template <typename Range>
tail(Range &&range)->tail<decltype(driftmeta::adl_begin(range))>;

} // namespace drift