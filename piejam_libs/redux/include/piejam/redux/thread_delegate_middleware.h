// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/concepts.h>
#include <piejam/redux/functors.h>
#include <piejam/redux/make_middleware.h>
#include <piejam/redux/middleware_functors.h>

#include <memory>
#include <thread>

namespace piejam::redux
{

template <class DelegateMethod>
class thread_delegate_middleware
{
public:
    thread_delegate_middleware(
            std::thread::id delegate_to_id,
            DelegateMethod delegate_method)
        : m_delegate_to_id(delegate_to_id)
        , m_delegate_method(std::move(delegate_method))
    {
    }

    template <class State, concepts::cloneable Action>
    void
    operator()(middleware_functors<State, Action> const& mw_fs, Action const& a)
    {
        if (std::this_thread::get_id() == m_delegate_to_id)
        {
            mw_fs.next(a);
        }
        else
        {
            m_delegate_method(
                    [&mw_fs, clone = a.clone()]() { mw_fs.dispatch(*clone); });
        }
    }

private:
    std::thread::id m_delegate_to_id;
    DelegateMethod m_delegate_method;
};

template <class DelegateMethod>
thread_delegate_middleware(std::thread::id, DelegateMethod)
        -> thread_delegate_middleware<DelegateMethod>;

} // namespace piejam::redux
