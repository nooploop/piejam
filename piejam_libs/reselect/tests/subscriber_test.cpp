// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/reselect/subscriber.h>

#include <gtest/gtest.h>

namespace piejam::reselect::test
{

namespace
{

struct state
{
    int x{};
};

} // namespace

TEST(subscriber, observe_will_also_notify)
{
    auto sel = selector<int, state>(&state::x);

    state st{5};
    subscriber<state> sut([&st]() -> state const& { return st; });

    int x{};
    auto sub = sut.observe<int>(sel, [&x](int st_x) { x = st_x; });

    EXPECT_EQ(5, x);
}

TEST(subscriber,
     second_notify_wont_trigger_the_handler_if_selected_value_stays_same)
{
    auto sel = selector<int, state>(&state::x);

    state st;
    subscriber<state> sut([&st]() -> state const& { return st; });

    int handler_called{};
    auto sub =
            sut.observe<int>(sel, [&handler_called](int) { ++handler_called; });

    sut.notify(st);
    sut.notify(st);

    EXPECT_EQ(1, handler_called);
}

TEST(subscriber,
     handler_is_triggered_again_if_selected_value_changed_since_last_notify)
{
    auto sel = selector<int, state>(&state::x);

    state st;
    subscriber<state> sut([&st]() -> state const& { return st; });

    int handler_called{};
    auto sub =
            sut.observe<int>(sel, [&handler_called](int) { ++handler_called; });

    st.x = 5;
    sut.notify(st);

    EXPECT_EQ(2, handler_called);
}

TEST(subscriber, handler_not_called_when_unsubscribed)
{
    auto sel = selector<int, state>(&state::x);

    state st;
    subscriber<state> sut([&st]() -> state const& { return st; });

    int handler_called{};

    {
        auto sub = sut.observe<int>(sel, [&handler_called](int) {
            ++handler_called;
        });

        EXPECT_EQ(1, handler_called);
    }

    sut.notify(st);
    EXPECT_EQ(1, handler_called);
}

TEST(subscriber, renotify_when_a_new_observer_connected_while_notifying)
{
    auto sel = selector<int, state>(&state::x);

    state st;
    subscriber<state> sut([&st]() -> state const& { return st; });

    int handler1_called{};
    int handler2_called{};

    std::unique_ptr<subscription> inner_sub;
    auto sub = sut.observe<int>(sel, [&](int) {
        inner_sub = std::make_unique<subscription>(
                sut.observe<int>(sel, [&handler2_called](int) {
                    ++handler2_called;
                }));
        ++handler1_called;
    });

    EXPECT_EQ(1, handler1_called);
    EXPECT_EQ(1, handler2_called);
}

TEST(subscriber, observe_once_will_get_current_state)
{
    auto sel = selector<int, state>(&state::x);

    state st{.x = 5};
    subscriber<state> sut([&st]() -> state const& { return st; });

    EXPECT_EQ(5, sut.observe_once(sel));
}

} // namespace piejam::reselect::test
