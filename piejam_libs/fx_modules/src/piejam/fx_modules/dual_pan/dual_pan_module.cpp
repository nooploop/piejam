// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/dual_pan/dual_pan_module.h>

#include <piejam/fx_modules/dual_pan/dual_pan_internal_id.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/runtime/parameter_value_to_string.h>
#include <piejam/runtime/ui_parameter_descriptors_map.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::dual_pan
{

auto
make_module(runtime::internal_fx_module_factory_args const& args)
        -> runtime::fx::module
{
    using namespace std::string_literals;

    runtime::parameter_factory ui_params_factory{args.params, args.ui_params};

    return runtime::fx::module{
            .fx_instance_id = internal_id(),
            .name = box("Dual Pan"s),
            .bus_type = args.bus_type,
            .parameters = box(runtime::fx::module_parameters{
                    {to_underlying(parameter_key::left_pan),
                     ui_params_factory.make_parameter(
                             runtime::float_parameter{
                                     .default_value = -1.f,
                                     .min = -1.f,
                                     .max = 1.f,
                                     .to_normalized =
                                             &runtime::parameter::
                                                     to_normalized_linear,
                                     .from_normalized =
                                             &runtime::parameter::
                                                     from_normalized_linear,
                             },
                             {
                                     .name = box("Left Pan"s),
                                     .value_to_string =
                                             &runtime::
                                                     float_parameter_value_to_string,
                                     .bipolar = true,
                             })},
                    {to_underlying(parameter_key::right_pan),
                     ui_params_factory.make_parameter(
                             runtime::float_parameter{
                                     .default_value = 1.f,
                                     .min = -1.f,
                                     .max = 1.f,
                                     .to_normalized =
                                             &runtime::parameter::
                                                     to_normalized_linear,
                                     .from_normalized =
                                             &runtime::parameter::
                                                     from_normalized_linear,
                             },
                             {
                                     .name = box("Right Pan"s),
                                     .value_to_string =
                                             &runtime::
                                                     float_parameter_value_to_string,
                                     .bipolar = true,
                             })},
            }),
            .streams = {}};
}

} // namespace piejam::fx_modules::dual_pan
