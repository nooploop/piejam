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

#include <piejam/runtime/components/fx_gain.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/value_input_processor.h>
#include <piejam/runtime/fx/gain.h>
#include <piejam/runtime/fx/module.h>

#include <fmt/format.h>

#include <array>

namespace piejam::runtime::components
{

namespace
{

class fx_gain final : public audio::engine::component
{
public:
    fx_gain(processors::make_parameter_input_processor_f<
                    parameter::float_> const& proc_factory,
            fx::module const& fx_mod,
            std::string_view const& name)
        : m_gain_input_proc(proc_factory(
                  fx_mod.parameters
                          .at(static_cast<std::size_t>(
                                  fx::gain_parameter_key::gain))
                          .id,
                  fmt::format("gain {}", name)))
        , m_amplifier(audio::components::make_stereo_amplifier(
                  fmt::format("amp {}", name)))
    {
    }

    auto inputs() const -> endpoints override { return m_amplifier->inputs(); }

    auto outputs() const -> endpoints override
    {
        return m_amplifier->outputs();
    }

    auto event_inputs() const -> endpoints override
    {
        return m_amplifier->event_inputs();
    }

    auto event_outputs() const -> endpoints override { return {}; }

    void connect(audio::engine::graph& g) override
    {
        m_amplifier->connect(g);

        g.add_event_wire(
                {*m_gain_input_proc, 0},
                m_amplifier->event_inputs()[0]);
    }

private:
    std::shared_ptr<audio::engine::processor> m_gain_input_proc;
    std::unique_ptr<audio::engine::component> m_amplifier;
};

} // namespace

auto
make_fx_gain(
        processors::make_parameter_input_processor_f<parameter::float_> const&
                proc_factory,
        fx::module const& fx_mod,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<fx_gain>(proc_factory, fx_mod, name);
}

} // namespace piejam::runtime::components
