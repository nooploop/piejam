// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter_maps_access.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime
{

class fx_parameter_factory
{
public:
    fx_parameter_factory(parameter_maps& params, fx::parameters_t& fx_params)
        : m_params{params}
        , m_fx_params{fx_params}
    {
    }

    template <class P>
    auto make_parameter(P&& param, fx::parameter fx_param) const
    {
        auto param_id = add_parameter(m_params, std::forward<P>(param));
        m_fx_params.emplace(param_id, std::move(fx_param));
        return param_id;
    }

private:
    parameter_maps& m_params;
    fx::parameters_t& m_fx_params;
};

} // namespace piejam::runtime
