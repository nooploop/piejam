// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/functional/overload.h>

#include <tuple>
#include <utility>

namespace piejam::runtime::ui
{

template <class Visitor>
struct visitable_action_interface
{
    using visitor_t = Visitor;

    virtual ~visitable_action_interface() = default;

    virtual void visit(visitor_t&) const = 0;

    bool operator==(visitable_action_interface const&) const noexcept = default;
};

template <class Action, class Base>
struct visitable_action : virtual Base
{
    void visit(typename Base::visitor_t& v) const override
    {
        v(static_cast<Action const&>(*this));
    }

    bool operator==(visitable_action const&) const noexcept = default;
};

template <class Action>
struct single_action_visitor_interface
{
    virtual ~single_action_visitor_interface() = default;

    virtual void operator()(Action const&) = 0;
};

template <class... Action>
struct action_visitor_interface
    : private single_action_visitor_interface<Action>...
{
    virtual ~action_visitor_interface() = default;

    using actions_t = std::tuple<Action...>;

    using single_action_visitor_interface<Action>::operator()...;
};

namespace detail
{

template <class Derived, class IVisitor, class Action>
struct single_action_visitor : virtual IVisitor
{
    void operator()(Action const& a) override
    {
        static_cast<Derived&>(*this).f(a);
    }
};

template <class F, class IVisitor, class... Action>
struct action_visitor final
    : detail::single_action_visitor<
              action_visitor<F, IVisitor, Action...>,
              IVisitor,
              Action>...
{
    using detail::single_action_visitor<
            action_visitor<F, IVisitor, Action...>,
            IVisitor,
            Action>::operator()...;

    F f;

    action_visitor(F&& f)
        : f(std::move(f))
    {
    }
};

template <class F, class IVisitor, class ActionTuple>
struct to_action_visitor;

template <class F, class IVisitor, class... Actions>
struct to_action_visitor<F, IVisitor, std::tuple<Actions...>>
{
    using type = action_visitor<F, IVisitor, Actions...>;
};

template <class F, class IVisitor>
using to_action_visitor_t =
        typename to_action_visitor<F, IVisitor, typename IVisitor::actions_t>::
                type;

} // namespace detail

template <class IVisitor, class... Fs>
auto
make_action_visitor(Fs&&... fs)
{
    using F = decltype(overload{std::forward<Fs>(fs)...});
    return detail::to_action_visitor_t<F, IVisitor>(
            overload{std::forward<Fs>(fs)...});
}

} // namespace piejam::runtime::ui
