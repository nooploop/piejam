// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/redux/batch_middleware.h>

#include <gtest/gtest.h>

namespace piejam::redux
{

namespace
{

struct tst_action
{
    bool is_batch_action{};
};

struct batch_action : tst_action
{
    batch_action() { is_batch_action = true; }

    bool empty() const { return batched_actions.empty(); }
    auto begin() const { return batched_actions.begin(); }
    auto end() const { return batched_actions.end(); }

    std::vector<tst_action> batched_actions;
};

auto
as_batch_action(tst_action const& a)
{
    return a.is_batch_action ? static_cast<batch_action const*>(&a) : nullptr;
}

struct batch_middleware_test : testing::Test
{
    bool m_batching{};
    std::size_t batched_count{};
    std::size_t unbatched_count{};

    batch_middleware<tst_action> sut{m_batching, [this](auto const&) {
                                         if (m_batching)
                                             ++batched_count;
                                         else
                                             ++unbatched_count;
                                     }};
};

TEST_F(batch_middleware_test, invoke_batched_actions_in_sequence)
{
    batch_action action;
    action.batched_actions.emplace_back();
    action.batched_actions.emplace_back();
    action.batched_actions.emplace_back();

    sut.operator()(action);

    EXPECT_EQ(2u, batched_count);
    EXPECT_EQ(1u, unbatched_count);
}

} // namespace

} // namespace piejam::redux
