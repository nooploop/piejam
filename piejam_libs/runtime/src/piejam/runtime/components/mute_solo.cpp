// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/mute_solo.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/processors/mute_solo_processor.h>

#include <array>

namespace piejam::runtime::components
{

namespace
{

class mute_solo final : public audio::engine::component
{
public:
    explicit mute_solo(std::string_view const name)
        : m_mute_solo_proc(processors::make_mute_solo_processor(name))
        , m_mute_solo_amp(audio::components::make_stereo_amplifier("mute"))
    {
    }

    [[nodiscard]]
    auto inputs() const -> endpoints override
    {
        return m_mute_solo_amp->inputs();
    }

    [[nodiscard]]
    auto outputs() const -> endpoints override
    {
        return m_mute_solo_amp->outputs();
    }

    [[nodiscard]]
    auto event_inputs() const -> endpoints override
    {
        return m_event_inputs;
    }

    [[nodiscard]]
    auto event_outputs() const -> endpoints override
    {
        return {};
    }

    void connect(audio::engine::graph& g) const override
    {
        m_mute_solo_amp->connect(g);

        g.event.insert(
                {*m_mute_solo_proc, 0},
                m_mute_solo_amp->event_inputs()[0]);
    }

private:
    std::unique_ptr<audio::engine::processor> m_mute_solo_proc;
    std::unique_ptr<audio::engine::component> m_mute_solo_amp;

    std::array<audio::engine::graph_endpoint, 3> m_event_inputs{
            {{*m_mute_solo_proc, 0},
             {*m_mute_solo_proc, 1},
             {*m_mute_solo_proc, 2}}};
};

} // namespace

auto
make_mute_solo(std::string_view const name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<mute_solo>(name);
}

} // namespace piejam::runtime::components
