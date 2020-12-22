// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/runtime/persistence_middleware.h>

#include <piejam/runtime/actions/load_app_config.h>
#include <piejam/runtime/actions/load_session.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/actions/save_session.h>
#include <piejam/runtime/persistence/access.h>
#include <piejam/runtime/persistence/session.h>

#include <boost/assert.hpp>

namespace piejam::runtime
{

persistence_middleware::persistence_middleware(
        get_state_f get_state,
        dispatch_f dispatch,
        next_f next)
    : m_get_state(std::move(get_state))
    , m_dispatch(std::move(dispatch))
    , m_next(std::move(next))
{
    BOOST_ASSERT(m_get_state);
    BOOST_ASSERT(m_next);
}

void
persistence_middleware::operator()(action const& a)
{
    if (auto action = dynamic_cast<actions::persistence_action const*>(&a))
    {
        auto v = ui::make_action_visitor<actions::persistence_action_visitor>(
                [this](auto&& a) { process_persistence_action(a); });

        action->visit(v);
    }
    else
    {
        m_next(a);
    }
}

template <>
void
persistence_middleware::process_persistence_action(
        actions::load_app_config const& a)
{
    persistence::load_app_config(a.file, m_dispatch);
}

template <>
void
persistence_middleware::process_persistence_action(
        actions::save_app_config const& a)
{
    persistence::save_app_config(a.file, m_get_state());
}

template <>
void
persistence_middleware::process_persistence_action(
        actions::load_session const& a)
{
    persistence::load_session(a.file, m_dispatch);
}

template <>
void
persistence_middleware::process_persistence_action(
        actions::save_session const& a)
{
    persistence::save_session(a.file, m_get_state());
}

} // namespace piejam::runtime
