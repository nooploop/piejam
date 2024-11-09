// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/redux/selector.h>

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

TEST(selector, make_selector_from_pmd)
{
    auto sut = selector<int, state>(&state::x);
    state st{5};
    EXPECT_EQ(5, sut(st));
}

TEST(selector, make_selector_from_lambda)
{
    auto sut = selector<int, state>([](state const& st) { return st.x + 5; });
    state st{5};
    EXPECT_EQ(10, sut(st));
}

TEST(selector, combine)
{
    auto sub_sel = selector<int, state>(&state::x);
    auto combined =
            combine([](int a, int b) { return a + b; }, sub_sel, sub_sel);
    state st{5};
    EXPECT_EQ(combined(st), 10);
}

} // namespace piejam::redux::test
