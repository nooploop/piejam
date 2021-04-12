// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/fx/scope.h>

#include <piejam/entity_map.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter_maps_access.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::fx
{

namespace
{

struct scope_defaults
{
    static constexpr float default_value{1.f};
    static constexpr float min{0.001f};
    static constexpr float max{8.f};
};

} // namespace

auto
make_scope_module(
        parameters_t& fx_params,
        parameter_maps& params,
        audio_streams_cache& streams) -> module
{
    using namespace std::string_literals;

    auto add_resolution_param = [&]() {
        auto id = add_parameter(
                params,
                runtime::parameter::float_{
                        .default_value = scope_defaults::default_value,
                        .min = scope_defaults::min,
                        .max = scope_defaults::max,
                        .to_normalized =
                                &runtime::parameter::to_normalized_linear,
                        .from_normalized =
                                &runtime::parameter::from_normalized_linear});
        fx_params.emplace(
                id,
                parameter{
                        .name = "Resolution"s,
                        .value_to_string =
                                parameter_value_to_string([](float const x) {
                                    return fmt::format("{:.3f} sec", x);
                                })});
        return id;
    };

    return module{
            .fx_instance_id = internal::scope,
            .name = "Scope"s,
            .parameters =
                    fx::module_parameters{
                            {static_cast<parameter_key>(
                                     scope_parameter_key::resolution),
                             add_resolution_param()}},
            .streams = module_streams{
                    {static_cast<stream_key>(scope_stream_key::left_right),
                     streams.add(audio_stream_buffer(std::in_place, 2))}}};
}

} // namespace piejam::runtime::fx
