// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/fx/gain.h>

#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/map.h>

#include <fmt/format.h>

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
make_gain_module(parameters_t& fx_params, float_parameters& float_params)
        -> module
{
    using namespace std::string_literals;

    auto add_param = [&](auto&& p, auto&& name) {
        auto id = float_params.add(std::forward<decltype(p)>(p));
        fx_params.emplace(
                id,
                parameter{
                        .name = std::forward<decltype(name)>(name),
                        .value_to_string =
                                parameter_value_to_string(&to_dB_string)});
        return id;
    };

    return module{
            .fx_instance_id = internal::gain,
            .name = "gain"s,
            .parameters = fx::module_parameters{
                    {static_cast<std::size_t>(gain_parameter_key::gain),
                     add_param(
                             runtime::parameter::float_{
                                     .default_value =
                                             gain_defaults::default_value,
                                     .min = gain_defaults::min,
                                     .max = gain_defaults::max,
                                     .to_normalized = runtime::parameter::
                                             to_normalized_db<dB_ival>,
                                     .from_normalized =
                                             &runtime::parameter::
                                                     from_normalized_db<
                                                             dB_ival>},
                             "gain"s)}}};
}

} // namespace piejam::runtime::fx
