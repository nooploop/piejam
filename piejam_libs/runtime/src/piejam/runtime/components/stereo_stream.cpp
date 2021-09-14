// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/stereo_stream.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/stream_processor.h>
#include <piejam/runtime/processors/stream_processor_factory.h>

#include <array>

namespace piejam::runtime::components
{

namespace
{

class stereo_stream final : public audio::engine::component
{
public:
    stereo_stream(
            audio_stream_id const stream_id,
            processors::stream_processor_factory& stream_proc_factory,
            std::size_t const buffer_capacity_per_channel,
            std::string_view const& name)
        : m_stream_proc(stream_proc_factory.make_processor(
                  stream_id,
                  2,
                  buffer_capacity_per_channel,
                  name))
    {
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return {}; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(audio::engine::graph& g) const override
    {
        g.audio.insert(
                {.proc = *m_left_identity, .port = 0},
                {.proc = *m_stream_proc, .port = 0});
        g.audio.insert(
                {.proc = *m_right_identity, .port = 0},
                {.proc = *m_stream_proc, .port = 1});
    }

private:
    std::unique_ptr<audio::engine::processor> m_left_identity{
            audio::engine::make_identity_processor()};
    std::unique_ptr<audio::engine::processor> m_right_identity{
            audio::engine::make_identity_processor()};
    std::shared_ptr<audio::engine::processor> m_stream_proc;

    std::array<audio::engine::graph_endpoint, 2> m_inputs{
            audio::engine::graph_endpoint{*m_left_identity, 0},
            audio::engine::graph_endpoint{*m_right_identity, 0}};
    std::array<audio::engine::graph_endpoint, 2> m_outputs{
            audio::engine::graph_endpoint{*m_left_identity, 0},
            audio::engine::graph_endpoint{*m_right_identity, 0}};
};

} // namespace

auto
make_stereo_stream(
        audio_stream_id const stream_id,
        processors::stream_processor_factory& stream_proc_factory,
        std::size_t const buffer_capacity_per_channel,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<stereo_stream>(
            stream_id,
            stream_proc_factory,
            buffer_capacity_per_channel,
            name);
}

} // namespace piejam::runtime::components
