// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
        fx::simple_ladspa_processor_factory const& ladspa_fx_proc_factory,
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
                    },
                    [](fx::unavailable_ladspa_id const&)
                            -> std::unique_ptr<audio::engine::component> {
                        return nullptr;
                    }},
            fx_mod.fx_instance_id);
}

} // namespace piejam::runtime::components
