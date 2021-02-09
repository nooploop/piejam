// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <boost/mp11/algorithm.hpp>

#include <variant>
#include <vector>

namespace piejam::runtime::actions
{

struct request_parameters_update final
    : ui::cloneable_action<request_parameters_update, action>
    , visitable_engine_action<request_parameters_update>
{
    using parameter_id_t = boost::mp11::mp_rename<
            boost::mp11::mp_transform<parameter::id_t, parameters_t>,
            std::variant>;

    std::vector<parameter_id_t> param_ids;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
