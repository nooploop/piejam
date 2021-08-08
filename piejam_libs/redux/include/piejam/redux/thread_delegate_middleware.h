// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/redux/cloneable.h>
#include <piejam/redux/functors.h>
#include <piejam/redux/make_middleware.h>

#include <memory>
#include <thread>

namespace piejam::redux
{

template <class DelegateMethod, class Action>
    requires cloneable<Action>
class thread_delegate_middleware
{
public:
    thread_delegate_middleware(
            std::thread::id delegate_to_id,
            DelegateMethod delegate_method,
            dispatch_f<Action> dispatch,
            next_f<Action> next)
        : m_delegate_to_id(delegate_to_id)
        , m_delegate_method(std::move(delegate_method))
        , m_dispatch(std::move(dispatch))
        , m_next(std::move(next))
    {
    }

    void operator()(Action const& a)
    {
        if (std::this_thread::get_id() == m_delegate_to_id)
        {
            m_next(a);
        }
        else
        {
            m_delegate_method([dispatch = m_dispatch, clone = a.clone()]() {
                dispatch(*clone);
            });
        }
    }

private:
    std::thread::id m_delegate_to_id;
    DelegateMethod m_delegate_method;
    dispatch_f<Action> m_dispatch;
    next_f<Action> m_next;
};

template <class DelegateMethod, class Action>
thread_delegate_middleware(
        std::thread::id,
        DelegateMethod,
        dispatch_f<Action>,
        next_f<Action>) -> thread_delegate_middleware<DelegateMethod, Action>;

template <class DelegateMethod>
struct make_thread_delegate_middleware
{
    make_thread_delegate_middleware(
            std::thread::id delegate_to_id,
            DelegateMethod delegate_method)
        : m_delegate_to_id(delegate_to_id)
        , m_delegate_method(std::move(delegate_method))
    {
    }

    template <class GetState, class Dispatch, class Action>
    auto operator()(GetState&&, Dispatch&& dispatch, next_f<Action> next) const
    {
        return make_middleware<
                thread_delegate_middleware<DelegateMethod, Action>>(
                m_delegate_to_id,
                m_delegate_method,
                std::move(dispatch),
                std::move(next));
    }

private:
    std::thread::id m_delegate_to_id;
    DelegateMethod m_delegate_method;
};

template <class DelegateMethod>
make_thread_delegate_middleware(std::thread::id, DelegateMethod)
        -> make_thread_delegate_middleware<DelegateMethod>;

} // namespace piejam::redux
