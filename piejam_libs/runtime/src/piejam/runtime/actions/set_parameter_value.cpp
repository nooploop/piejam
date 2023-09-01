// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/set_parameter_value.h>

#include <piejam/functional/in_interval.h>
#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
parameter_value_is_in_range(bool_parameter const&, bool)
{
    return true;
}

template <class Parameter>
auto
parameter_value_is_in_range(
        Parameter const& param,
        parameter::value_type_t<Parameter> value)
{
    return in_closed(value, param.min, param.max);
}

template <class Parameter>
void
set_parameter_value<Parameter>::reduce(state& st) const
{
    auto& desc = st.params[id];
    BOOST_ASSERT(parameter_value_is_in_range(desc.param, value));
    desc.value.set(value);
}

template struct set_parameter_value<bool_parameter>;
template struct set_parameter_value<int_parameter>;
template struct set_parameter_value<float_parameter>;

} // namespace piejam::runtime::actions
