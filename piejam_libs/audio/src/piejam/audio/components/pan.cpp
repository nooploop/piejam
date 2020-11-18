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

#include <piejam/audio/components/level_meter.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_converter_processor.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/pan_balance_processor.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/pair.h>

#include <fmt/format.h>

#include <array>

namespace piejam::audio::components
{

namespace
{

class pan final : public engine::component
{
public:
    pan(std::string_view name)
        : m_pan_proc(engine::make_pan_processor(fmt::format("pan {}", name)))
        , m_amp_comp(
                  make_stereo_split_amplifier(fmt::format("pan_amp {}", name)))
    {
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(engine::graph& g) override
    {
        m_amp_comp->connect(g);

        g.add_event_wire({*m_pan_proc, 0}, m_amp_comp->event_inputs()[0]);
        g.add_event_wire({*m_pan_proc, 1}, m_amp_comp->event_inputs()[1]);
    }

private:
    std::unique_ptr<engine::processor> m_pan_proc;
    std::unique_ptr<engine::component> m_amp_comp;

    std::array<engine::graph_endpoint, 2> m_inputs{
            {m_amp_comp->inputs()[0], m_amp_comp->inputs()[1]}};
    std::array<engine::graph_endpoint, 2> m_outputs{
            {m_amp_comp->outputs()[0], m_amp_comp->outputs()[1]}};
    std::array<engine::graph_endpoint, 1> m_event_inputs{{{*m_pan_proc, 0}}};
};

} // namespace

auto
make_pan(std::string_view name) -> std::unique_ptr<engine::component>
{
    return std::make_unique<pan>(name);
}

} // namespace piejam::audio::components
