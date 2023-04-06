// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/request_parameters_update.h>

#include <piejam/runtime/state.h>

#include <boost/mp11/tuple.hpp>

namespace piejam::runtime::actions
{

auto
request_parameters_update::empty() const noexcept -> bool
{
    bool result{true};

    boost::mp11::tuple_for_each(param_ids, [&result](auto const& ids) {
        result = result && ids.empty();
    });

    return result;
}

} // namespace piejam::runtime::actions
