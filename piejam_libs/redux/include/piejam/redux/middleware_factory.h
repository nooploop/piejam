// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/concepts.h>
#include <piejam/redux/middleware_functors.h>

#include <memory>

namespace piejam::redux
{

template <class State, class Action>
class middleware_factory
{
public:
    template <class MW>
    class middleware_factory_facade
    {
    public:
        template <class... Args>
        middleware_factory_facade(Args&&... args)
            : m_middleware{std::make_shared<MW>(std::forward<Args>(args)...)}
        {
        }

        template <
                concepts::get_state<State> GetState,
                concepts::dispatch<Action> Dispatch,
                concepts::next<Action> Next>
        auto
        operator()(GetState&& get_state, Dispatch&& dispatch, Next&& next) const
        {
            return [mw_fs =
                            middleware_functors<State, Action>{
                                    std::forward<GetState>(get_state),
                                    std::forward<Dispatch>(dispatch),
                                    std::forward<Next>(next)},
                    mw = m_middleware](Action const& a) { (*mw)(mw_fs, a); };
        }

    private:
        std::shared_ptr<MW> m_middleware;
    };

    template <class MW, class... Args>
    static auto make(Args&&... args) -> middleware_factory_facade<MW>
    {
        return middleware_factory_facade<MW>{std::forward<Args>(args)...};
    }
};

} // namespace piejam::redux
