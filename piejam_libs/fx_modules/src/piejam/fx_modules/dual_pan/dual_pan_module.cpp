// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/dual_pan/dual_pan_module.h>

#include <piejam/fx_modules/dual_pan/dual_pan_internal_id.h>

#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/bool_descriptor.h>
#include <piejam/runtime/parameter/float_descriptor.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::dual_pan
{

static constexpr auto s_pan_to_noramlized =
        &runtime::parameter::to_normalized_linear_static<-1.f, 1.f>;
static constexpr auto s_pan_from_noramlized =
        &runtime::parameter::from_normalized_linear_static<-1.f, 1.f>;

auto
make_module(runtime::internal_fx_module_factory_args const& args)
        -> runtime::fx::module
{
    using namespace std::string_literals;

    runtime::parameter_factory params_factory{args.params};

    return runtime::fx::module{
            .fx_instance_id = internal_id(),
            .name = box("Dual Pan"s),
            .bus_type = args.bus_type,
            .parameters = box(runtime::fx::module_parameters{
                    {to_underlying(parameter_key::mute_left),
                     params_factory.make_parameter(runtime::bool_parameter{
                             .name = box("Mute Left"s),
                             .default_value = false})},
                    {to_underlying(parameter_key::pan_left),
                     params_factory.make_parameter(runtime::float_parameter{
                             .name = box("Pan Left"s),
                             .default_value = -1.f,
                             .min = -1.f,
                             .max = 1.f,
                             .bipolar = true,
                             .to_normalized = s_pan_to_noramlized,
                             .from_normalized = s_pan_from_noramlized,
                     })},
                    {to_underlying(parameter_key::pan_right),
                     params_factory.make_parameter(runtime::float_parameter{
                             .name = box("Pan Right"s),
                             .default_value = 1.f,
                             .min = -1.f,
                             .max = 1.f,
                             .bipolar = true,
                             .to_normalized = s_pan_to_noramlized,
                             .from_normalized = s_pan_from_noramlized,
                     })},
                    {to_underlying(parameter_key::mute_right),
                     params_factory.make_parameter(runtime::bool_parameter{
                             .name = box("Mute Right"s),
                             .default_value = false})},
            }),
            .streams = {}};
}

} // namespace piejam::fx_modules::dual_pan
