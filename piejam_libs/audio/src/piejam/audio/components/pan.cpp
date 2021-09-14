// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    auto inputs() const -> endpoints override { return m_amp_comp->inputs(); }
    auto outputs() const -> endpoints override { return m_amp_comp->outputs(); }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(engine::graph& g) const override
    {
        m_amp_comp->connect(g);

        g.event.insert({*m_pan_proc, 0}, m_amp_comp->event_inputs()[0]);
        g.event.insert({*m_pan_proc, 1}, m_amp_comp->event_inputs()[1]);
    }

private:
    std::unique_ptr<engine::processor> m_pan_proc;
    std::unique_ptr<engine::component> m_amp_comp;

    std::array<engine::graph_endpoint, 1> m_event_inputs{{{*m_pan_proc, 0}}};
};

} // namespace

auto
make_pan(std::string_view name) -> std::unique_ptr<engine::component>
{
    return std::make_unique<pan>(name);
}

} // namespace piejam::audio::components
