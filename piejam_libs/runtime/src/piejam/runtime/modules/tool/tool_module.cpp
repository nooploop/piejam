// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/tool/tool_module.h>

#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/runtime/ui_parameter_descriptors_map.h>
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

    static constexpr auto min_gain{std::pow(10.f, min / 20.f)};
    static constexpr auto max_gain{std::pow(10.f, max / 20.f)};

    static constexpr auto to_normalized = &parameter::to_normalized_dB<dB_ival>;
    static constexpr auto from_normalized =
            &parameter::from_normalized_dB<dB_ival>;
};

auto
to_dB_string(float x) -> std::string
{
    return fmt::format("{:.1f} dB", std::log10(x) * 20.f);
}

} // namespace

auto
make_module(
        audio::bus_type const bus_type,
        parameters_map& params,
        ui_parameter_descriptors_map& ui_params) -> fx::module
{
    using namespace std::string_literals;

    parameter_factory ui_params_factory{params, ui_params};

    return fx::module{
            .fx_instance_id = fx::internal::tool,
            .name = "Tool"s,
            .bus_type = bus_type,
            .parameters =
                    fx::module_parameters{
                            {to_underlying(parameter_key::gain),
                             ui_params_factory.make_parameter(
                                     float_parameter{
                                             .default_value = 1.f,
                                             .min = dB_ival::min_gain,
                                             .max = dB_ival::max_gain,
                                             .to_normalized =
                                                     dB_ival::to_normalized,
                                             .from_normalized =
                                                     dB_ival::from_normalized},
                                     {.name = "Gain"s,
                                      .value_to_string = &to_dB_string})}},
            .streams = {}};
}

} // namespace piejam::runtime::modules::tool
