// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <boost/mp11/algorithm.hpp>

#include <variant>
#include <vector>

namespace piejam::runtime::actions
{

struct update_parameter_values final
    : ui::cloneable_action<update_parameter_values, action>
{
    template <class P>
    using parameter_id_value_pair =
            std::pair<parameter::id_t<P>, typename P::value_type>;

    using parameter_id_value_t = boost::mp11::mp_rename<
            boost::mp11::mp_transform<parameter_id_value_pair, parameters_t>,
            std::variant>;

    using parameter_values_t = std::vector<parameter_id_value_t>;

    parameter_values_t values;

    auto reduce(state const& st) const -> state;
};

} // namespace piejam::runtime::actions
