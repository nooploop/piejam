// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/update_parameter_values.h>

#include <piejam/algorithm/for_each_visit.h>
#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

auto
update_parameter_values::reduce(state const& st) const -> state
{
    auto new_st = st;

    algorithm::for_each_visit(values, [&new_st](auto const& id_value) {
        new_st.params.set(id_value.first, id_value.second);
    });

    return new_st;
}

} // namespace piejam::runtime::actions
