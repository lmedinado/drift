/* a simple task system along the lines of Sean Parent's "concurrency" talk.
 *
 * Copyright (c) 2023 - present, Leandro Medina de Oliveira
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

#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

namespace drift {

template <class T>
std::decay_t<T> decay_copy(T &&v) {
    return std::forward<T>(v);
}

template <typename T>
class mustached_future {
private:
    std::future<T> exec_ = {};
    std::promise<T> promise_ = {};

public:
    mustached_future() noexcept = default;
    explicit mustached_future(std::future<T> &&f) : exec_(std::move(f)) {}

    mustached_future(const mustached_future &) = delete;
    mustached_future &operator=(const mustached_future &) = delete;

    mustached_future(mustached_future &&rhs) noexcept = default;
    mustached_future &operator=(mustached_future &&rhs) noexcept = default;

    bool valid() const noexcept { return exec_.valid(); }
    void swap(mustached_future &other) noexcept {
        using std::swap;
        swap(exec_, other.exec_);
        swap(promise_, other.promise_);
    }

    /* getting the result */
    std::future<T> get_future() { return promise_.get_future(); }

    /* execution */
    void operator()() {
        try {
            if constexpr (std::is_void_v<T>) {
                exec_.get();
                promise_.set_value();
            } else {
                promise_.set_value(exec_.get());
            }
        } catch (...) {
            try {
                promise_.set_exception(std::current_exception());
            } catch (...) {} // set_exception() may throw too
        }
    }
};

template <typename T = void>
class notification_queue {
public:
    using t_lock = std::unique_lock<std::mutex>;
    using t_thunk = mustached_future<T>;

private:
    std::deque<t_thunk> q_;
    bool done_ = false;
    std::mutex mutex_;
    std::condition_variable ready_;

public:
    void finish() {
        {
            auto lock = t_lock{mutex_};
            done_ = true;
        }
        ready_.notify_all();
    }

    bool try_pop(t_thunk &x) {
        auto lock = t_lock(mutex_, std::try_to_lock);

        if (!lock or q_.empty())
            return false;

        x = std::move(q_.front());
        q_.pop_front();
        return true;
    }

    bool pop(t_thunk &x) {
        auto lock = t_lock{mutex_};
        while (q_.empty() and not done_)
            ready_.wait(lock);

        if (q_.empty())
            return false;
        x = std::move(q_.front());
        q_.pop_front();
        return true;
    }

    template <typename U, std::enable_if_t<std::is_void_v<T> or std::is_constructible_v<T, U>, int> = 0>
    std::optional<std::future<T>> try_push(std::future<U> &&f) {
        auto fut = [&]() -> std::optional<std::future<T>> {
            auto lock = t_lock(mutex_, std::try_to_lock);
            if (!lock)
                return std::nullopt;

            return q_.emplace_back(std::move(f)).get_future();
        }();
        ready_.notify_one();
        return fut;
    }

    template <typename U, std::enable_if_t<std::is_void_v<T> or std::is_constructible_v<T, U>, int> = 0>
    std::future<T> push(std::future<U> &&f) {
        auto fut = [&] {
            auto lock = t_lock{mutex_};
            return q_.emplace_back(std::move(f)).get_future();
        }();
        ready_.notify_one();
        return fut;
    }
};

/* single queue */
template <typename T = void>
class single_queue {
private:
    using t_thunk = typename notification_queue<T>::t_thunk;

    const unsigned n_workers_ = std::thread::hardware_concurrency();
    std::vector<std::thread> workers_;
    notification_queue<T> q_;

    void run() {
        while (true) {
            t_thunk f;
            if (!q_.pop(f))
                break;

            f();
        }
    }

public:
    using future_t = std::future<T>;

    single_queue(unsigned n_workers = std::thread::hardware_concurrency())
      : n_workers_(n_workers) {
        for (auto n = 0u; n != n_workers_; ++n) {
            workers_.emplace_back([&] { run(); });
        }
    }
    ~single_queue() {
        q_.finish();
        for (auto &t : workers_)
            t.join();
    }
    single_queue &operator=(single_queue &&) = delete;

    template <typename F, typename... Args>
    future_t async(F &&f, Args &&...args) {
        return q_.push(std::async(std::launch::deferred, std::forward<F>(f),
                                  std::forward<Args>(args)...));
    }
};

/* multi-queue */
template <typename T = void>
class multi_queue {
private:
    using t_thunk = typename notification_queue<T>::t_thunk;

    const unsigned n_workers_ = std::thread::hardware_concurrency();
    std::vector<std::thread> workers_;
    std::vector<notification_queue<T>> q_{n_workers_};
    std::atomic<unsigned> index_{0};

    void run(unsigned i) {
        while (true) {
            t_thunk f;

            if (!q_[i].pop(f))
                break;

            f();
        }
    }

public:
    using future_t = std::future<T>;

    multi_queue(unsigned n_workers = std::thread::hardware_concurrency())
      : n_workers_(n_workers) {
        for (auto n = 0u; n != n_workers_; ++n) {
            workers_.emplace_back([&, n] { run(n); });
        }
    }
    ~multi_queue() {
        for (auto &q : q_)
            q.finish();
        for (auto &t : workers_)
            t.join();
    }
    multi_queue &operator=(multi_queue &&) = delete;

    template <typename F, typename... Args>
    future_t async(F &&f, Args &&...args) {
        auto i = index_++;
        return q_[i % n_workers_].push(std::async(std::launch::deferred, std::forward<F>(f),
                                                  std::forward<Args>(args)...));
    }
};

/* task stealing */
template <typename T = void>
class task_stealing_queue {
private:
    using t_thunk = typename notification_queue<T>::t_thunk;

    unsigned const n_workers_ = std::thread::hardware_concurrency();
    static constexpr auto k = 2;
    std::vector<std::thread> workers_;
    std::vector<notification_queue<T>> q_{n_workers_};
    std::atomic<unsigned> index_{0};

    void run(unsigned i) {
        while (true) {
            t_thunk f;
            for (auto n = 0u; n != n_workers_; ++n) {
                if ((q_[(i + n) % n_workers_].try_pop(f)))
                    break;
            }
            if (!f.valid() and !(q_[i].pop(f)))
                break;

            f();
        }
    }

public:
    using future_t = std::future<T>;

    task_stealing_queue(unsigned n_workers = std::thread::hardware_concurrency())
      : n_workers_(n_workers) {
        for (auto n = 0u; n != n_workers_; ++n) {
            workers_.emplace_back([&, n] { run(n); });
        }
    }
    ~task_stealing_queue() {
        for (auto &q : q_)
            q.finish();
        for (auto &t : workers_)
            t.join();
    }
    void wait() {
        for (auto &q : q_)
            q.finish();
    }

    task_stealing_queue &operator=(task_stealing_queue &&) = delete;

    template <typename F, typename... Args>
    future_t async(F &&f, Args &&...args) {

        auto g = [&]() {
            if constexpr (std::is_void_v<T>) {
                return std::async(
                    std::launch::deferred,
                    [f = decay_copy(f),
                     args = std::make_tuple(decay_copy(std::forward<Args>(args))...)]() -> void {
                        std::apply(std::move(f), args);
                        return;
                    });
            } else {
                return std::async(
                    std::launch::deferred,
                    [f = decay_copy(f)](Args &&...args) -> T {
                        return (T)std::invoke(std::move(f), std::forward<Args>(args)...);
                    },
                    std::forward<Args>(args)...);
            }
        }();
        auto i = index_++;
        for (auto n = 0u; n != n_workers_ * k; ++n) {
            if (auto p = q_[(i + n) % n_workers_].try_push(std::move(g)); p)
                return std::move(*p);
        }
        return std::move(q_[i % n_workers_].push(std::move(g)));
    }
};

} // namespace drift
