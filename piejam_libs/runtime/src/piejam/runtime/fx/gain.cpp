// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/runtime/fx/gain.h>

#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/map.h>

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
                        .unit = parameter_unit::dB});
        return id;
    };

    return module{
            .fx_type_id = internal::gain,
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
