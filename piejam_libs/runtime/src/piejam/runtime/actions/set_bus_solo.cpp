// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/set_bus_solo.h>

#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

template <>
auto
set_bus_solo<io_direction::input>::reduce(state const& st) const -> state
{
    auto new_st = st;

    BOOST_ASSERT(index < new_st.mixer_state.inputs->size());
    auto const& id = new_st.mixer_state.inputs.get()[index];

    new_st.mixer_state.input_solo_id =
            id == new_st.mixer_state.input_solo_id ? mixer::bus_id{} : id;

    return new_st;
}

} // namespace piejam::runtime::actions
