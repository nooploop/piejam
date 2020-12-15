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

#include <piejam/runtime/session_middleware.h>

#include <piejam/runtime/actions/load_app_config.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/config_access.h>

#include <boost/assert.hpp>

namespace piejam::runtime
{

session_middleware::session_middleware(
        locations const& locs,
        get_state_f get_state,
        dispatch_f dispatch,
        next_f next)
    : m_get_state(std::move(get_state))
    , m_dispatch(std::move(dispatch))
    , m_next(std::move(next))
    , m_locations(locs)
{
    BOOST_ASSERT(m_get_state);
    BOOST_ASSERT(m_next);
    BOOST_ASSERT(!m_locations.config_dir.empty());
}

void
session_middleware::operator()(action const& a)
{
    if (dynamic_cast<actions::save_app_config const*>(&a))
    {
        config_access::save(m_locations, m_get_state());
    }
    else if (dynamic_cast<actions::load_app_config const*>(&a))
    {
        config_access::load(m_locations, m_dispatch);
    }
    else
    {
        m_next(a);
    }
}

} // namespace piejam::runtime
