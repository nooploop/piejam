// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/set_parameter_value.h>

#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

template <class Parameter>
void
set_parameter_value<Parameter>::reduce(state& st) const
{
    st.params.set(id, value);
}

template struct set_parameter_value<bool_parameter>;
template struct set_parameter_value<int_parameter>;
template struct set_parameter_value<float_parameter>;

} // namespace piejam::runtime::actions
