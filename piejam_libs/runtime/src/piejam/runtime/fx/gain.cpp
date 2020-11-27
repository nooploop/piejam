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
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/map.h>

namespace piejam::runtime::fx
{

auto
make_gain_module(float_parameters& float_params) -> module
{
    module mod;
    mod.fx_type = type::gain;

    mod.parameters.emplace(
            static_cast<std::size_t>(gain_parameter_key::gain),
            parameter{
                    .id = float_params.add(runtime::parameter::float_{
                            .default_value = 1.f,
                            .min = 0.f,
                            .max = 8.f}),
                    .name = "gain"});

    return mod;
}

} // namespace piejam::runtime::fx
