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

#include <piejam/runtime/components/make_fx.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/entity_id.h>
#include <piejam/functional/overload.h>
#include <piejam/runtime/components/fx_gain.h>
#include <piejam/runtime/components/fx_ladspa.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>

namespace piejam::runtime::components
{

auto
make_fx(fx::module const& fx_mod,
        fx::get_parameter_name const& get_fx_param_name,
        fx::ladspa_processor_factory const& ladspa_fx_proc_factory,
        parameter_processor_factory& param_procs,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::visit(
            overload{
                    [&](fx::internal fx_type)
                            -> std::unique_ptr<audio::engine::component> {
                        switch (fx_type)
                        {
                            case fx::internal::gain:
                                return components::make_fx_gain(
                                        fx_mod,
                                        param_procs,
                                        name);

                            default:
                                return nullptr;
                        }
                    },
                    [&](fx::ladspa_instance_id id)
                            -> std::unique_ptr<audio::engine::component> {
                        return components::make_fx_ladspa(
                                fx_mod,
                                get_fx_param_name,
                                [&, id]() {
                                    return ladspa_fx_proc_factory(id);
                                },
                                param_procs);
                    }},
            fx_mod.fx_type_id);
}

} // namespace piejam::runtime::components
