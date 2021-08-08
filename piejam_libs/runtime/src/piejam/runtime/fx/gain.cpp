// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/fx/gain.h>

#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter_maps_access.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::fx
{

namespace
{

struct dB_ival
{
    static constexpr auto min{-24.f};
    static constexpr auto max{24.f};
};

struct gain_defaults
{
    static constexpr float default_value{1.f};
    static constexpr float min{std::pow(10.f, dB_ival::min / 20.f)};
    static constexpr float max{std::pow(10.f, dB_ival::max / 20.f)};
};

auto
to_dB_string(float x) -> std::string
{
    return fmt::format("{:.1f} dB", (std::log(x) / std::log(10)) * 20.f);
}

} // namespace

auto
make_gain_module(parameters_t& fx_params, parameter_maps& params) -> module
{
    using namespace std::string_literals;

    auto gain_param_id = add_parameter(
            params,
            runtime::parameter::float_{
                    .default_value = gain_defaults::default_value,
                    .min = gain_defaults::min,
                    .max = gain_defaults::max,
                    .to_normalized =
                            runtime::parameter::to_normalized_db<dB_ival>,
                    .from_normalized =
                            &runtime::parameter::from_normalized_db<dB_ival>});
    fx_params.emplace(
            gain_param_id,
            parameter{
                    .name = "Gain"s,
                    .value_to_string =
                            parameter_value_to_string(&to_dB_string)});

    return module{
            .fx_instance_id = internal::gain,
            .name = "Gain"s,
            .parameters =
                    fx::module_parameters{
                            {to_underlying(gain_parameter_key::gain),
                             gain_param_id}},
            .streams = {}};
}

} // namespace piejam::runtime::fx
