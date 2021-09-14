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

class stereo_balance final : public engine::component
{
public:
    stereo_balance(std::string_view name)
        : m_stereo_balance_proc(engine::make_stereo_balance_processor(
                  fmt::format("stereo_balance {}", name)))
        , m_amp_comp(make_stereo_split_amplifier(
                  fmt::format("stereo_balance_amp {}", name)))
    {
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return m_event_inputs; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(engine::graph& g) const override
    {
        m_amp_comp->connect(g);

        g.event.insert(
                {*m_stereo_balance_proc, 0},
                m_amp_comp->event_inputs()[0]);
        g.event.insert(
                {*m_stereo_balance_proc, 1},
                m_amp_comp->event_inputs()[1]);
    }

private:
    std::unique_ptr<engine::processor> m_stereo_balance_proc;
    std::unique_ptr<engine::component> m_amp_comp;

    std::array<engine::graph_endpoint, 2> m_inputs{
            m_amp_comp->inputs()[0],
            m_amp_comp->inputs()[1]};
    std::array<engine::graph_endpoint, 2> m_outputs{
            {m_amp_comp->outputs()[0], m_amp_comp->outputs()[1]}};
    std::array<engine::graph_endpoint, 1> m_event_inputs{
            {{*m_stereo_balance_proc, 0}}};
};

} // namespace

auto
make_stereo_balance(std::string_view name) -> std::unique_ptr<engine::component>
{
    return std::make_unique<stereo_balance>(name);
}

} // namespace piejam::audio::components
