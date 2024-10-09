// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/audio_engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <boost/container/flat_set.hpp>

#include <tuple>

namespace piejam::runtime::actions
{

struct request_parameters_update final
    : ui::cloneable_action<request_parameters_update, action>
    , visitable_audio_engine_action<request_parameters_update>
{
    template <class Parameter>
    using parameter_id_set =
            boost::container::flat_set<parameter::id_t<Parameter>>;

    using parameter_id_sets = boost::mp11::mp_rename<
            boost::mp11::mp_transform<parameter_id_set, parameter_ids_t>,
            std::tuple>;

    parameter_id_sets param_ids;

    template <class Parameter>
    void push_back(parameter::id_t<Parameter> const id)
    {
        auto& ids = std::get<parameter_id_set<Parameter>>(param_ids);
        ids.emplace_hint(ids.end(), id);
    }

    auto empty() const noexcept -> bool;
};

} // namespace piejam::runtime::actions
