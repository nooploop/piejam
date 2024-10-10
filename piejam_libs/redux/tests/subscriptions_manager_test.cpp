// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/redux/subscriptions_manager.h>

#include <gtest/gtest.h>

namespace piejam::redux::test
{

namespace
{

struct state
{
    int x{};
};

} // namespace

TEST(subscriptions_manager, after_observe_notify_will_call_the_handler)
{
    auto sel = selector<int, state>(&state::x);

    state st;
    subscriber<state> subscrbr([&st]() -> state const& { return st; });

    subscriptions_manager<std::size_t> sut;

    std::size_t id{1};
    int handler_called{};
    sut.observe(id, subscrbr, sel, [&handler_called](int) {
        ++handler_called;
    });

    EXPECT_EQ(1, handler_called);

    st.x = 5;
    subscrbr.notify(st);

    EXPECT_EQ(2, handler_called);
}

TEST(subscriptions_manager, erase_will_prevent_further_notifications)
{
    auto sel = selector<int, state>(&state::x);

    state st;
    subscriber<state> subscrbr([&st]() -> state const& { return st; });

    subscriptions_manager<std::size_t> sut;

    std::size_t id{1};
    int handler_called{};
    sut.observe(id, subscrbr, sel, [&handler_called](int) {
        ++handler_called;
    });

    EXPECT_EQ(1, handler_called);

    sut.erase(id);

    st.x = 5;
    subscrbr.notify(st);

    EXPECT_EQ(1, handler_called);
}

TEST(subscriptions_manager, destruction_will_disconnect_all_observers)
{
    auto sel = selector<int, state>(&state::x);

    state st;
    subscriber<state> subscrbr([&st]() -> state const& { return st; });

    int handler_called{};

    {
        subscriptions_manager<std::size_t> sut;

        std::size_t id{1};
        sut.observe(id, subscrbr, sel, [&handler_called](int) {
            ++handler_called;
        });

        EXPECT_EQ(1, handler_called);
    }

    st.x = 5;
    subscrbr.notify(st);

    EXPECT_EQ(1, handler_called);
}

} // namespace piejam::redux::test
