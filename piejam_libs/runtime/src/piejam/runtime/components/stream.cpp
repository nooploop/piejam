// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/stream.h>

#include <piejam/runtime/processors/stream_processor_factory.h>

#include <piejam/audio/components/identity.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/engine/stream_processor.h>

namespace piejam::runtime::components
{

namespace
{

class stream final : public audio::engine::component
{

public:
    stream(
        audio_stream_id const stream_id,
        processors::stream_processor_factory& stream_proc_factory,
        std::size_t const num_channels,
        std::size_t const buffer_capacity_per_channel,
        std::string_view const name)
        : m_stream_proc{stream_proc_factory.make_processor(
              stream_id,
              num_channels,
              buffer_capacity_per_channel,
              name)}
        , m_input_identity{audio::components::make_identity(num_channels)}
    {
    }

    auto inputs() const -> endpoints override
    {
        return m_input_identity->inputs();
    }

    auto outputs() const -> endpoints override
    {
        return m_input_identity->outputs();
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
        m_input_identity->connect(g);

        audio::engine::connect(g, *m_input_identity, *m_stream_proc);
    }

private:
    std::shared_ptr<audio::engine::processor> m_stream_proc;
    std::unique_ptr<audio::engine::component> m_input_identity;
};

} // namespace

auto
make_stream(
    audio_stream_id const stream_id,
    processors::stream_processor_factory& stream_proc_factory,
    std::size_t const num_channels,
    std::size_t const buffer_capacity_per_channel,
    std::string_view const name) -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<stream>(
        stream_id,
        stream_proc_factory,
        num_channels,
        buffer_capacity_per_channel,
        name);
}

} // namespace piejam::runtime::components
