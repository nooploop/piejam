// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/make_fx.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/pair.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/internal_fx_component_factory.h>
#include <piejam/runtime/ladspa_fx/ladspa_fx_component.h>

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/hof/match.hpp>

namespace piejam::runtime::components
{

namespace
{

auto
make_internal_fx(
        fx::internal_id id,
        fx::module const& fx_mod,
        audio::sample_rate const sample_rate,
        parameter_processor_factory& param_procs,
        processors::stream_processor_factory& stream_procs,
        std::string_view const name)
        -> std::unique_ptr<audio::engine::component>
{
    return internal_fx_component_factories::lookup(id)({
            .fx_mod = fx_mod,
            .sample_rate = sample_rate,
            .param_procs = param_procs,
            .stream_procs = stream_procs,
            .name = name,
    });
}

} // namespace

auto
make_fx(fx::module const& fx_mod,
        fx::get_parameter_name const& get_fx_param_name,
        fx::simple_ladspa_processor_factory const& ladspa_fx_proc_factory,
        parameter_processor_factory& param_procs,
        processors::stream_processor_factory& stream_procs,
        audio::sample_rate const sample_rate,
        std::string_view const name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::visit(
            boost::hof::match(
                    [&](fx::internal_id id)
                            -> std::unique_ptr<audio::engine::component> {
                        return make_internal_fx(
                                id,
                                fx_mod,
                                sample_rate,
                                param_procs,
                                stream_procs,
                                name);
                    },
                    [&](ladspa::instance_id id)
                            -> std::unique_ptr<audio::engine::component> {
                        return ladspa_fx::make_component(
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
                    }),
            fx_mod.fx_instance_id);
}

} // namespace piejam::runtime::components
