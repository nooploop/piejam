// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/ui/action.h>

#include <piejam/redux/middleware_functors.h>

namespace piejam::runtime::ui
{

template <class State, class Data>
class generic_action_visitor
{
public:
    using mw_fs_t = redux::middleware_functors<State, action>;

    generic_action_visitor(mw_fs_t const& mw_fs, Data& data)
        : m_mw_fs{mw_fs}
        , data{data}
    {
    }

    auto get_state() const -> State const&
    {
        return m_mw_fs.get_state();
    }

    void dispatch(action const& a)
    {
        m_mw_fs.dispatch(a);
    }

    void next(action const& a)
    {
        m_mw_fs.next(a);
    }

private:
    mw_fs_t const& m_mw_fs;

public:
    Data& data;
};

template <class State>
class generic_action_visitor<State, void>
{
public:
    using mw_fs_t = redux::middleware_functors<State, action>;

    generic_action_visitor(mw_fs_t const& mw_fs)
        : m_mw_fs{mw_fs}
    {
    }

    auto get_state() const -> State const&
    {
        return m_mw_fs.get_state();
    }

    void dispatch(action const& a)
    {
        m_mw_fs.dispatch(a);
    }

    void next(action const& a)
    {
        m_mw_fs.next(a);
    }

private:
    mw_fs_t const& m_mw_fs;
};

} // namespace piejam::runtime::ui
