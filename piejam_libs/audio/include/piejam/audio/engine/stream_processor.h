// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/audio_spsc_ring_buffer.h>
#include <piejam/audio/engine/named_processor.h>

#include <string_view>

namespace piejam::audio::engine
{

class stream_processor final : public named_processor
{
public:
    stream_processor(
            std::size_t num_channels,
            std::size_t capacity_per_channel,
            std::string_view const& name = {});

    auto type_name() const -> std::string_view override { return "stream"; }

    auto num_inputs() const -> std::size_t override { return m_num_channels; }
    auto num_outputs() const -> std::size_t override { return m_num_channels; }

    auto event_inputs() const -> event_ports override { return {}; }
    auto event_outputs() const -> event_ports override { return {}; }

    void process(process_context const& ctx) override;

    template <class F>
    void consume(F&& f)
    {
        m_buffer.consume(std::forward<F>(f));
    }

private:
    std::size_t m_num_channels;
    audio_spsc_ring_buffer m_buffer;
    std::vector<float> m_interleave_buffer;
};

} // namespace piejam::audio::engine
