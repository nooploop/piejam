// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/audio_engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

template <class Parameter>
struct set_parameter_value final
    : ui::cloneable_action<set_parameter_value<Parameter>, reducible_action>
    , visitable_audio_engine_action<set_parameter_value<Parameter>>
{
    template <class V>
    set_parameter_value(parameter::id_t<Parameter> id, V&& value)
        : id(id)
        , value(std::forward<V>(value))
    {
    }

    void reduce(state&) const override;

    parameter::id_t<Parameter> id{};
    typename Parameter::value_type value{};
};

template <class Parameter, class V>
set_parameter_value(parameter::id_t<Parameter>, V&&)
        -> set_parameter_value<Parameter>;

} // namespace piejam::runtime::actions
