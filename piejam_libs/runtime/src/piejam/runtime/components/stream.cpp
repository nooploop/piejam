// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/stream.h>

#include <piejam/runtime/processors/stream_processor_factory.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/stream_processor.h>
#include <piejam/range/indices.h>
#include <piejam/range/iota.h>

#include <vector>

namespace piejam::runtime::components
{

namespace
{

class stream final : public audio::engine::component
{

public:
    stream(audio_stream_id const stream_id,
           processors::stream_processor_factory& stream_proc_factory,
           std::size_t const num_channels,
           std::size_t const buffer_capacity_per_channel,
           std::string_view const name)
        : m_stream_proc{stream_proc_factory.make_processor(
                  stream_id,
                  num_channels,
                  buffer_capacity_per_channel,
                  name)}
        , m_identity{algorithm::transform_to_vector(
                  range::iota(num_channels),
                  [](auto) {
                      return audio::engine::make_identity_processor();
                  })}
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
        for (auto port : range::indices(m_identity))
        {
            g.audio.insert(
                    {.proc = *m_identity[port], .port = 0},
                    {.proc = *m_stream_proc, .port = port});
        }
    }

private:
    std::shared_ptr<audio::engine::processor> m_stream_proc;
    std::vector<std::unique_ptr<audio::engine::processor>> m_identity;

    std::vector<audio::engine::graph_endpoint> m_inputs{
            algorithm::transform_to_vector(
                    range::indices(m_identity),
                    [this](auto port) {
                        return audio::engine::graph_endpoint{
                                .proc = *m_identity[port],
                                .port = 0};
                    })};
    std::vector<audio::engine::graph_endpoint> m_outputs{m_inputs};
};

} // namespace

auto
make_stream(
        audio_stream_id const stream_id,
        processors::stream_processor_factory& stream_proc_factory,
        std::size_t const num_channels,
        std::size_t const buffer_capacity_per_channel,
        std::string_view const name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<stream>(
            stream_id,
            stream_proc_factory,
            num_channels,
            buffer_capacity_per_channel,
            name);
}

} // namespace piejam::runtime::components
