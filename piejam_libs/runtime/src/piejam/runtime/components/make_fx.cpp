// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/make_fx.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/modules/filter/filter_component.h>
#include <piejam/runtime/modules/ladspa_fx/ladspa_fx_component.h>
#include <piejam/runtime/modules/scope/scope_component.h>
#include <piejam/runtime/modules/spectrum/spectrum_component.h>
#include <piejam/runtime/modules/tool/tool_component.h>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::runtime::components
{

namespace
{

auto
make_internal_fx(
        fx::internal fx_type,
        fx::module const& fx_mod,
        parameter_processor_factory& param_procs,
        processors::stream_processor_factory& stream_procs,
        audio::sample_rate const& sample_rate,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::component>
{
    switch (fx_type)
    {
        case fx::internal::tool:
            return modules::tool::make_component(fx_mod, param_procs, name);

        case fx::internal::filter:
            return modules::filter::make_component(
                    fx_mod,
                    sample_rate,
                    param_procs,
                    stream_procs,
                    name);

        case fx::internal::scope:
            return modules::scope::make_component(
                    fx_mod,
                    sample_rate,
                    stream_procs);

        case fx::internal::spectrum:
            return modules::spectrum::make_component(
                    fx_mod,
                    sample_rate,
                    stream_procs);
    }

    BOOST_ASSERT_MSG(false, "Unknown internal fx");
    return nullptr;
}

} // namespace

auto
make_fx(fx::module const& fx_mod,
        fx::get_parameter_name const& get_fx_param_name,
        fx::simple_ladspa_processor_factory const& ladspa_fx_proc_factory,
        parameter_processor_factory& param_procs,
        processors::stream_processor_factory& stream_procs,
        audio::sample_rate const& sample_rate,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::visit(
            boost::hof::match(
                    [&](fx::internal fx_type)
                            -> std::unique_ptr<audio::engine::component> {
                        return make_internal_fx(
                                fx_type,
                                fx_mod,
                                param_procs,
                                stream_procs,
                                sample_rate,
                                name);
                    },
                    [&](ladspa::instance_id id)
                            -> std::unique_ptr<audio::engine::component> {
                        return modules::ladspa_fx::make_component(
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
