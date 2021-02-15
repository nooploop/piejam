// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/set_bus_solo.h>

#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

template <io_direction D>
auto
set_bus_solo<D>::reduce(state const& st) const -> state
{
    auto new_st = st;

    BOOST_ASSERT(index < mixer::bus_ids<D>(st.mixer_state).size());
    auto const id = mixer::bus_ids<D>(st.mixer_state)[index];

    mixer::solo_id<D>(new_st.mixer_state) =
            id == mixer::solo_id<D>(st.mixer_state) ? mixer::bus_id{} : id;

    return new_st;
}

template struct set_bus_solo<io_direction::input>;
template struct set_bus_solo<io_direction::output>;

} // namespace piejam::runtime::actions
