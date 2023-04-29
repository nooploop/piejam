// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/tool/tool_module.h>

#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx_parameter_factory.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::modules::tool
{

namespace
{

struct dB_ival
{
    static constexpr auto min{-24.f};
    static constexpr auto max{24.f};
};

auto
to_dB_string(float x) -> std::string
{
    return fmt::format("{:.1f} dB", (std::log(x) / std::log(10)) * 20.f);
}

} // namespace

auto
make_module(fx_parameter_factory const& fx_params_factory) -> fx::module
{
    using namespace std::string_literals;

    // clang-format off
    return fx::module{
            .fx_instance_id = fx::internal::tool,
            .name = "Tool"s,
            .parameters =
                    fx::module_parameters{
                            {to_underlying(parameter_key::gain),
                             fx_params_factory.make_parameter(
                                     runtime::float_parameter{
                                             .default_value = 1.f,
                                             .min = std::pow(10.f, dB_ival::min / 20.f),
                                             .max = std::pow(10.f, dB_ival::max / 20.f),
                                             .to_normalized = &runtime::parameter::to_normalized_dB<dB_ival>,
                                             .from_normalized = &runtime::parameter::from_normalized_dB<dB_ival>},
                                     fx::parameter{
                                             .name = "Gain"s,
                                             .value_to_string = &to_dB_string})}},
            .streams = {}};
    // clang-format on
}

} // namespace piejam::runtime::modules::tool
