/* dysfunction is a dynamic type-erased overload set.
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

#include <functional>

namespace drift {

template <typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <typename R, typename... Args>
struct dysfunction : private std::function<R(Args)>... {
    using std::function<R(Args)>::operator()...;

    template <typename... Ts>
    dysfunction(Ts &&...ts) : dysfunction(overloaded{std::forward<Ts>(ts)...}) {}

private:
    template <typename... Ts>
    dysfunction(overloaded<Ts...> &&ov)
      : te_ov([=]() mutable -> void * { return &ov; }),
        std::function<R(Args)>([this](Args &&args) {
            return (*static_cast<overloaded<Ts...> *>(te_ov()))(std::forward<Args>(args));
        })... {}

    std::function<void *(void)> te_ov;
};

} // namespace drift
