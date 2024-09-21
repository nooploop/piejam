// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/redux/thread_delegate_middleware.h>

#include <gtest/gtest.h>

#include <future>
#include <vector>

namespace piejam::redux::test
{

namespace
{

struct queaction
{
    int x;

    [[nodiscard]] auto clone() const -> std::unique_ptr<queaction>
    {
        return std::make_unique<queaction>(queaction{x});
    }
};

using mw_fs_t = middleware_functors<int, queaction>;

TEST(thread_delegate_middleware,
     if_action_comes_on_required_thread_proceed_to_next)
{
    int next_result{};

    auto sut =
            thread_delegate_middleware(std::this_thread::get_id(), [](auto&&) {
                FAIL();
            });
    sut(mw_fs_t{[state = int{}]() -> int const& { return state; },
                [](queaction const&) { FAIL(); },
                [&next_result](queaction const& a) { next_result = a.x; }},
        queaction{23});

    EXPECT_EQ(23, next_result);
}

TEST(thread_delegate_middleware,
     if_action_comes_on_not_required_thread_delegate_to_required_one)
{
    int dispatch_result{};

    auto sut = thread_delegate_middleware(
            std::this_thread::get_id(),
            [](auto&& f) { f(); });

    auto f = std::async(std::launch::async, [&]() {
        sut(mw_fs_t{[state = int{}]() -> int const& { return state; },
                    [&dispatch_result](queaction const& a) {
                        dispatch_result = a.x;
                    },
                    [](queaction const&) { FAIL(); }},
            queaction{23});
    });
    f.wait();

    EXPECT_EQ(23, dispatch_result);
}

} // namespace
} // namespace piejam::redux::test
