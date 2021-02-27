// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/update_parameter_values.h>

#include <piejam/runtime/state.h>

#include <boost/mp11/tuple.hpp>

namespace piejam::runtime::actions
{

auto
update_parameter_values::reduce(state const& st) const -> state
{
    auto new_st = st;

    boost::mp11::tuple_for_each(values, [&new_st](auto const& id_value_pairs) {
        new_st.params.set(id_value_pairs);
    });

    return new_st;
}

auto
update_parameter_values::empty() const noexcept -> bool
{
    bool result{true};

    boost::mp11::tuple_for_each(values, [&result](auto const& v) {
        result = result && v.empty();
    });

    return result;
}

} // namespace piejam::runtime::actions
