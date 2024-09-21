// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/update_parameter_values.h>

#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/state.h>

#include <boost/mp11/tuple.hpp>

namespace piejam::runtime::actions
{

void
update_parameter_values::reduce(state& st) const
{
    boost::mp11::tuple_for_each(values, [&st](auto const& id_value_pairs) {
        for (auto const& [id, value] : id_value_pairs)
        {
            st.params[id].value.set(value);
        }
    });
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
