// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/parameter/bool_descriptor.h>
#include <piejam/runtime/parameter/float_descriptor.h>
#include <piejam/runtime/parameter/int_descriptor.h>
#include <piejam/runtime/parameter/stereo_level_descriptor.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <boost/container/flat_map.hpp>
#include <boost/mp11/algorithm.hpp>

#include <tuple>

namespace piejam::runtime::actions
{

struct update_parameter_values final
    : ui::cloneable_action<update_parameter_values, reducible_action>
{
    template <class Parameter>
    using id_value_map_t = boost::container::flat_map<
            parameter::id_t<Parameter>,
            parameter::value_type_t<Parameter>>;

    using parameter_values_t = boost::mp11::mp_rename<
            boost::mp11::mp_transform<id_value_map_t, parameters_t>,
            std::tuple>;

    parameter_values_t values;

    template <class P>
    void
    push_back(parameter::id_t<P> const id, typename P::value_type const value)
    {
        std::get<id_value_map_t<P>>(values).emplace(id, value);
    }

    [[nodiscard]]
    auto empty() const noexcept -> bool;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
