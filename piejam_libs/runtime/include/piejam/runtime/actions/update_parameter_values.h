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

#include <boost/container/flat_map.hpp>
#include <boost/mp11/algorithm.hpp>

#include <tuple>
#include <vector>

namespace piejam::runtime::actions
{

struct update_parameter_values final
    : ui::cloneable_action<update_parameter_values, reducible_action>
{
    using parameter_values_t = boost::mp11::mp_rename<
            boost::mp11::mp_transform<parameter::id_value_map_t, parameters_t>,
            std::tuple>;

    parameter_values_t values;

    template <class P>
    void
    push_back(parameter::id_t<P> const id, typename P::value_type const value)
    {
        std::get<parameter::id_value_map_t<P>>(values).emplace(id, value);
    }

    [[nodiscard]] auto empty() const noexcept -> bool;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
