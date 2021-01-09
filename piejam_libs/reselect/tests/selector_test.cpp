// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/reselect/selector.h>

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

TEST(selector, make_selector_from_pmd)
{
    auto sut = selector<int, state>(&state::x);
    state st{5};
    EXPECT_EQ(5, sut.get(st));
}

TEST(selector, make_selector_from_lambda)
{
    auto sut = selector<int, state>([](state const& st) { return st.x + 5; });
    state st{5};
    EXPECT_EQ(10, sut.get(st));
}

} // namespace piejam::reselect::test
