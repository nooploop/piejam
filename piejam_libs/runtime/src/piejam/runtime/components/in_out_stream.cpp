// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/stream.h>

#include <piejam/runtime/processors/stream_processor_factory.h>

#include <piejam/audio/components/identity.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/stream_processor.h>

namespace piejam::runtime::components
{

namespace
{

class in_out_stream final : public audio::engine::component
{

public:
    in_out_stream(
        std::unique_ptr<audio::engine::component> comp,
        std::shared_ptr<audio::engine::processor> stream_proc)
        : m_comp{std::move(comp)}
        , m_stream_proc{std::move(stream_proc)}
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_input_identity->inputs();
    }

    auto outputs() const -> endpoints override
    {
        return m_comp->outputs();
    }

    auto event_inputs() const -> endpoints override
    {
        return {};
    }

    auto event_outputs() const -> endpoints override
    {
        return {};
    }

    void connect(audio::engine::graph& g) const override
    {
        m_comp->connect(g);
        m_input_identity->connect(g);

        std::size_t port{};
        for (auto src : m_input_identity->outputs())
        {
            g.audio.insert(src, {.proc = *m_stream_proc, .port = port});
            g.audio.insert(src, m_comp->inputs()[port]);
            ++port;
        }

        for (auto src : m_comp->outputs())
        {
            g.audio.insert(src, {.proc = *m_stream_proc, .port = port});
            ++port;
        }
    }

private:
    std::unique_ptr<audio::engine::component> m_comp;
    std::shared_ptr<audio::engine::processor> m_stream_proc;

    std::unique_ptr<audio::engine::component> m_input_identity{
        audio::components::make_identity(m_comp->num_inputs())};
};

} // namespace

auto
wrap_with_in_out_stream(
    std::unique_ptr<audio::engine::component> comp,
    std::string_view name,
    runtime::audio_stream_id stream_id,
    runtime::processors::stream_processor_factory& stream_proc_factory,
    std::size_t const buffer_capacity_per_channel)
    -> std::unique_ptr<audio::engine::component>
{
    auto stream_proc = stream_proc_factory.make_processor(
        stream_id,
        comp->num_inputs() + comp->num_outputs(),
        buffer_capacity_per_channel,
        name);
    return std::make_unique<in_out_stream>(
        std::move(comp),
        std::move(stream_proc));
}

} // namespace piejam::runtime::components
