// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/redux/make_middleware.h>
#include <piejam/redux/queueing_middleware.h>
#include <piejam/redux/store.h>

#include <gtest/gtest.h>

#include <vector>

namespace piejam::redux::test
{

namespace
{

using state_t = std::vector<int>;

struct queaction
{
    int x;

    [[nodiscard]] auto clone() const -> std::unique_ptr<queaction>
    {
        return std::make_unique<queaction>(queaction{x});
    }
};

struct single_dispatch_middleware
{
    auto operator()(
            middleware_functors<state_t, queaction> const& mw_fs,
            queaction const& a)
    {
        if (dispatch)
        {
            mw_fs.dispatch(queaction{999});
            dispatch = false;
        }

        mw_fs.next(a);
    }

    bool dispatch{true};
};

using store_t = store<state_t, queaction>;

} // namespace

TEST(queueing_middleware, if_currently_not_dispatching_proceed_to_next)
{
    using make_mw_factory = middleware_factory<state_t, queaction>;

    store_t store{[](state_t& st, queaction const& a) { st.push_back(a.x); }};

    store.apply_middleware(
            make_mw_factory::make<queueing_middleware<queaction>>());

    store.dispatch(queaction{23});

    ASSERT_EQ(1u, store.state().size());
    EXPECT_EQ(23, store.state()[0]);
}

TEST(queueing_middleware,
     if_currently_dispatching_then_new_dispatched_action_is_queued)
{
    using make_mw_factory = middleware_factory<state_t, queaction>;

    store_t store{[](state_t& st, queaction const& a) { st.push_back(a.x); }};

    store.apply_middleware(make_mw_factory::make<single_dispatch_middleware>());

    store.apply_middleware(
            make_mw_factory::make<queueing_middleware<queaction>>());

    store.dispatch(queaction{23});

    ASSERT_EQ(2u, store.state().size());
    EXPECT_EQ(23, store.state()[0]);
    EXPECT_EQ(999, store.state()[1]);
}

} // namespace piejam::redux::test
