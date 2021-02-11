// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/middleware_functors.h>
#include <piejam/runtime/state.h>

#include <gmock/gmock.h>

namespace piejam::runtime::test
{

struct middleware_functors_mock
{
    MOCK_METHOD(state const&, get_state, (), (const));
    MOCK_METHOD(void, dispatch, (action const&));
    MOCK_METHOD(void, next, (action const&));
};

inline auto
make_middleware_functors(middleware_functors_mock& mock) -> middleware_functors
{
    return {[&mock]() -> state const& { return mock.get_state(); },
            [&mock](action const& a) { mock.dispatch(a); },
            [&mock](action const& a) { mock.next(a); }};
}

} // namespace piejam::runtime::test
