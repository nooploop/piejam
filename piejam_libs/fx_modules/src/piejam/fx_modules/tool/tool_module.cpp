// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/tool/tool_module.h>

#include <piejam/fx_modules/tool/tool_internal_id.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/runtime/ui_parameter_descriptors_map.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::tool
{

namespace
{

struct dB_ival
{
    static constexpr auto min{-24.f};
    static constexpr auto max{24.f};

    static constexpr auto min_gain{std::pow(10.f, min / 20.f)};
    static constexpr auto max_gain{std::pow(10.f, max / 20.f)};

    static constexpr auto to_normalized =
            &runtime::parameter::to_normalized_dB<dB_ival>;
    static constexpr auto from_normalized =
            &runtime::parameter::from_normalized_dB<dB_ival>;
};

auto
to_dB_string(float x) -> std::string
{
    return fmt::format("{:.1f} dB", std::log10(x) * 20.f);
}

} // namespace

auto
make_module(runtime::internal_fx_module_factory_args const& args)
        -> runtime::fx::module
{
    using namespace std::string_literals;

    runtime::parameter_factory ui_params_factory{args.params, args.ui_params};

    return runtime::fx::module{
            .fx_instance_id = internal_id(),
            .name = box_("Tool"s),
            .bus_type = args.bus_type,
            .parameters = unique_box_(runtime::fx::module_parameters{
                    {to_underlying(parameter_key::gain),
                     ui_params_factory.make_parameter(
                             runtime::float_parameter{
                                     .default_value = 1.f,
                                     .min = dB_ival::min_gain,
                                     .max = dB_ival::max_gain,
                                     .to_normalized = dB_ival::to_normalized,
                                     .from_normalized =
                                             dB_ival::from_normalized},
                             {.name = box_("Gain"s),
                              .value_to_string = &to_dB_string})}}),
            .streams = {}};
}

} // namespace piejam::fx_modules::tool
