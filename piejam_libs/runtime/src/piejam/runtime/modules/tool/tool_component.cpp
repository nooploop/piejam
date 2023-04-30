// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/tool/tool_component.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/modules/tool/tool_module.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

#include <array>

namespace piejam::runtime::modules::tool
{

namespace
{

class component final : public audio::engine::component
{
public:
    component(
            fx::module const& fx_mod,
            parameter_processor_factory& proc_factory,
            std::string_view const name)
        : m_gain_input_proc(processors::make_parameter_processor(
                  proc_factory,
                  fx_mod.parameters->at(to_underlying(parameter_key::gain)),
                  fmt::format("tool_gain {}", name)))
        , m_amplifier(
                  fx_mod.bus_type == audio::bus_type::mono
                          ? audio::components::make_mono_amplifier(
                                    fmt::format("tool {}", name))
                          : audio::components::make_stereo_amplifier(
                                    fmt::format("tool {}", name)))
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
make_component(
        fx::module const& fx_mod,
        parameter_processor_factory& proc_factory,
        std::string_view const name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<component>(fx_mod, proc_factory, name);
}

} // namespace piejam::runtime::modules::tool
