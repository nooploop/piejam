// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/stream.h>

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

template <std::size_t... Channel>
class stream final : public audio::engine::component
{
    static inline constexpr std::size_t num_channels{
            std::max({Channel...}) + 1};

public:
    stream(audio_stream_id const stream_id,
           processors::stream_processor_factory& stream_proc_factory,
           std::size_t const buffer_capacity_per_channel,
           std::string_view const name)
        : m_stream_proc(stream_proc_factory.make_processor(
                  stream_id,
                  num_channels,
                  buffer_capacity_per_channel,
                  name))
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_inputs;
    }

    auto outputs() const -> endpoints override
    {
        return m_outputs;
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
        (g.audio.insert(
                 {.proc = *m_identity[Channel], .port = 0},
                 {.proc = *m_stream_proc, .port = Channel}),
         ...);
    }

private:
    std::array<std::unique_ptr<audio::engine::processor>, num_channels>
            m_identity{
                    ((void)Channel,
                     audio::engine::make_identity_processor())...};
    std::shared_ptr<audio::engine::processor> m_stream_proc;

    std::array<audio::engine::graph_endpoint, num_channels> m_inputs{
            audio::engine::graph_endpoint{
                    .proc = *m_identity[Channel],
                    .port = 0}...};
    std::array<audio::engine::graph_endpoint, num_channels> m_outputs{
            audio::engine::graph_endpoint{
                    .proc = *m_identity[Channel],
                    .port = 0}...};
};

template <std::size_t... Channel>
auto
make_stream(
        std::index_sequence<Channel...>,
        audio_stream_id const stream_id,
        processors::stream_processor_factory& stream_proc_factory,
        std::size_t const buffer_capacity_per_channel,
        std::string_view const name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<stream<Channel...>>(
            stream_id,
            stream_proc_factory,
            buffer_capacity_per_channel,
            name);
}

} // namespace

auto
make_mono_stream(
        audio_stream_id const stream_id,
        processors::stream_processor_factory& stream_proc_factory,
        std::size_t const buffer_capacity_per_channel,
        std::string_view const name)
        -> std::unique_ptr<audio::engine::component>
{
    return make_stream(
            std::make_index_sequence<1>{},
            stream_id,
            stream_proc_factory,
            buffer_capacity_per_channel,
            name);
}

auto
make_stereo_stream(
        audio_stream_id const stream_id,
        processors::stream_processor_factory& stream_proc_factory,
        std::size_t const buffer_capacity_per_channel,
        std::string_view const name)
        -> std::unique_ptr<audio::engine::component>
{
    return make_stream(
            std::make_index_sequence<2>{},
            stream_id,
            stream_proc_factory,
            buffer_capacity_per_channel,
            name);
}

} // namespace piejam::runtime::components
