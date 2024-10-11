// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/utility/utility_component.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/fx_modules/utility/utility_module.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/internal_fx_component_factory.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::utility
{

namespace
{

class component final : public audio::engine::component
{
public:
    component(
            runtime::fx::module const& fx_mod,
            runtime::parameter_processor_factory& proc_factory,
            std::string_view const name)
        : m_gain_input_proc(
                  runtime::processors::find_or_make_parameter_processor(
                          proc_factory,
                          fx_mod.parameters->at(
                                  to_underlying(parameter_key::gain)),
                          fmt::format("utility_gain {}", name)))
        , m_amplifier(
                  fx_mod.bus_type == audio::bus_type::mono
                          ? audio::components::make_mono_amplifier(
                                    fmt::format("utility {}", name))
                          : audio::components::make_stereo_amplifier(
                                    fmt::format("utility {}", name)))
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_amplifier->inputs();
    }

    auto outputs() const -> endpoints override
    {
        return m_amplifier->outputs();
    }

    auto event_inputs() const -> endpoints override
    {
        return m_amplifier->event_inputs();
    }

    auto event_outputs() const -> endpoints override
    {
        return {};
    }

    void connect(audio::engine::graph& g) const override
    {
        m_amplifier->connect(g);

        g.event.insert({*m_gain_input_proc, 0}, m_amplifier->event_inputs()[0]);
    }

private:
    std::shared_ptr<audio::engine::processor> m_gain_input_proc;
    std::unique_ptr<audio::engine::component> m_amplifier;
};

} // namespace

auto
make_component(runtime::internal_fx_component_factory_args const& args)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<component>(
            args.fx_mod,
            args.param_procs,
            args.name);
}

} // namespace piejam::fx_modules::utility
