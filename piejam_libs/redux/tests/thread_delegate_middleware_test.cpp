// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/redux/thread_delegate_middleware.h>

#include <gtest/gtest.h>

#include <future>
#include <vector>

namespace piejam::redux::test
{

struct queaction
{
    int x;

    auto clone() const -> std::unique_ptr<queaction>
    {
        return std::make_unique<queaction>(queaction{x});
    }
};

TEST(thread_delegate_middleware,
     if_action_comes_on_required_thread_proceed_to_next)
{
    int next_result{};

    auto factory = make_thread_delegate_middleware(
            std::this_thread::get_id(),
            [](auto&&) { FAIL(); });
    auto sut = factory(
            []() { FAIL(); },
            [](queaction const&) { FAIL(); },
            redux::next_f<queaction>(
                    [&next_result](queaction const& a) { next_result = a.x; }));

    sut(queaction{23});

    EXPECT_EQ(23, next_result);
}

TEST(thread_delegate_middleware,
     if_action_comes_on_not_required_thread_delegate_to_required_one)
{
    int dispatch_result{};

    auto factory = make_thread_delegate_middleware(
            std::this_thread::get_id(),
            [](auto&& f) { f(); });
    auto sut = factory(
            []() { FAIL(); },
            [&dispatch_result](queaction const& a) { dispatch_result = a.x; },
            redux::next_f<queaction>([](queaction const&) { FAIL(); }));

    auto f = std::async(std::launch::async, [&sut]() { sut(queaction{23}); });
    f.wait();

    EXPECT_EQ(23, dispatch_result);
}

} // namespace piejam::redux::test
