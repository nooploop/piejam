// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/redux/store.h>

#include <gtest/gtest.h>

#include <vector>

namespace piejam::redux::test
{

using state_t = std::vector<int>;
using action_t = int;

TEST(store,
     dispatch_will_modify_the_state_and_call_the_subscriber_with_new_state)
{
    store<state_t, action_t> sut(
            [](state_t const& s, action_t const& a) {
                auto new_state = s;
                new_state.push_back(a);
                return new_state;
            },
            state_t());

    state_t subscribed;
    sut.subscribe([&subscribed](state_t const& s) { subscribed = s; });
    ASSERT_TRUE(subscribed.empty());

    sut.dispatch(action_t(5));

    EXPECT_EQ(state_t({5}), subscribed);
}

TEST(store,
     action_is_passed_to_middleware_to_reducer_and_state_passed_to_subscriber)
{
    store<state_t, action_t> sut(
            [](state_t const& s, action_t const& a) {
                auto new_state = s;
                new_state.push_back(a);
                return new_state;
            },
            state_t());

    bool middleware_called{};
    sut.apply_middleware([&middleware_called](
                                 auto /*get_state*/,
                                 auto /*dispatch*/,
                                 auto next) {
        return [next = std::move(next), &middleware_called](action_t const& a) {
            middleware_called = true;
            next(a);
        };
    });

    state_t subscribed;
    sut.subscribe([&subscribed](state_t const& s) { subscribed = s; });
    ASSERT_FALSE(middleware_called);
    ASSERT_TRUE(subscribed.empty());

    sut.dispatch(action_t(5));

    EXPECT_TRUE(middleware_called);
    EXPECT_EQ(state_t({5}), subscribed);
}

TEST(store, middlewares_are_called_in_reverse_order_of_their_registration)
{
    store<state_t, action_t> sut(
            [](state_t const& s, action_t const&) { return s; },
            {});

    int counter{};

    int m1_called{};
    sut.apply_middleware(
            [&m1_called,
             &counter](auto /*get_state*/, auto /*dispatch*/, auto next) {
                return [next = std::move(next), &m1_called, &counter](
                               action_t const& a) {
                    m1_called = ++counter;
                    next(a);
                };
            });

    int m2_called{};
    sut.apply_middleware(
            [&m2_called,
             &counter](auto /*get_state*/, auto /*dispatch*/, auto next) {
                return [next = std::move(next), &m2_called, &counter](
                               action_t const& a) {
                    m2_called = ++counter;
                    next(a);
                };
            });

    ASSERT_EQ(int{}, counter);
    ASSERT_EQ(int{}, m1_called);
    ASSERT_EQ(int{}, m2_called);

    sut.dispatch(action_t(5));

    EXPECT_EQ(2, counter);
    EXPECT_EQ(1, m2_called);
    EXPECT_EQ(2, m1_called);
}

struct move_only_middleware
{
    move_only_middleware() noexcept = default;
    move_only_middleware(move_only_middleware&&) noexcept = default;
    move_only_middleware(move_only_middleware const&) = delete;

    void operator()(action_t const&) {}
};

TEST(store, apply_move_only_middlewares)
{
    store<state_t, action_t> sut(
            [](state_t const& s, action_t const&) { return s; },
            {});

    sut.apply_middleware(
            [](auto /*get_state*/, auto /*dispatch*/, auto /*next*/) {
                // move only middlewares needs to be wrapped into a shared_ptr,
                // due std::function supports only copyable callables
                auto m = std::make_shared<move_only_middleware>();
                return [m](action_t const& a) { m->operator()(a); };
            });
}

} // namespace piejam::redux::test
