// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/set_parameter_value.h>

#include <piejam/runtime/parameter_maps_access.h>
#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

template <class Parameter>
auto
set_parameter_value<Parameter>::reduce(state const& st) const -> state
{
    auto new_st = st;
    runtime::set_parameter_value(new_st.params, id, value);
    return new_st;
}

template struct set_parameter_value<bool_parameter>;
template struct set_parameter_value<int_parameter>;
template struct set_parameter_value<float_parameter>;

} // namespace piejam::runtime::actions
