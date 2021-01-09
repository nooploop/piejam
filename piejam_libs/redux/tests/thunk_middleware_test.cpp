// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/redux/thunk_middleware.h>

#include <gtest/gtest.h>

namespace piejam::redux
{

namespace
{

struct tst_action
{
    bool is_thunk_action{};
};

struct thunk_action : tst_action
{
    thunk_action() { is_thunk_action = true; }

    std::function<void(get_state_f<int> const&, dispatch_f<tst_action> const&)>
            payload;

    void operator()(
            get_state_f<int> const& get_state,
            dispatch_f<tst_action> const& dispatch) const
    {
        payload(get_state, dispatch);
    }
};

auto
as_thunk_action(tst_action const& a)
{
    return a.is_thunk_action ? static_cast<thunk_action const*>(&a) : nullptr;
}

struct thunk_middleware_test : testing::Test
{
    int state{};
    thunk_middleware<int, tst_action> sut{
            [this]() -> int const& { return state; },
            [](auto const&) {},
            [](auto const&) {}};
};

TEST_F(thunk_middleware_test, invoke_thunk)
{
    int from_thunk{};
    thunk_action ta;
    ta.payload = [&from_thunk](auto&&, auto&&) { from_thunk = 5; };

    ASSERT_EQ(0, from_thunk);
    sut(ta);
    EXPECT_EQ(5, from_thunk);
}

} // namespace

} // namespace piejam::redux
