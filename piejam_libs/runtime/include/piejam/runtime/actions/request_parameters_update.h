// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <boost/container/flat_set.hpp>
#include <boost/mp11/algorithm.hpp>

#include <tuple>

namespace piejam::runtime::actions
{

struct request_parameters_update final
    : ui::cloneable_action<request_parameters_update, action>
    , visitable_engine_action<request_parameters_update>
{
    template <class Parameter>
    using parameter_ids_set_t =
            boost::container::flat_set<parameter::id_t<Parameter>>;

    using parameter_ids_t = boost::mp11::mp_rename<
            boost::mp11::mp_transform<parameter_ids_set_t, parameters_t>,
            std::tuple>;

    parameter_ids_t param_ids;

    template <class Parameter>
    void push_back(parameter::id_t<Parameter> const id)
    {
        auto& ids = std::get<parameter_ids_set_t<Parameter>>(param_ids);
        ids.emplace_hint(ids.end(), id);
    }

    auto empty() const noexcept -> bool;
};

} // namespace piejam::runtime::actions
