// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/audio_spsc_ring_buffer.h>
#include <piejam/audio/engine/named_processor.h>

#include <mipp.h>

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
    auto num_outputs() const -> std::size_t override { return 0; }

    auto event_inputs() const -> event_ports override { return {}; }
    auto event_outputs() const -> event_ports override { return {}; }

    void process(process_context const& ctx) override;

    template <class F>
    void consume(F&& f)
    {
        m_buffer.consume(std::forward<F>(f));
    }

private:
    using stream_fn_t = void (stream_processor::*)(process_context const&);

    void stream_1(process_context const&);
    void stream_2(process_context const&);
    void stream_4(process_context const&);
    void stream_n(process_context const&);
    static auto get_stream_fn(std::size_t num_channels) -> stream_fn_t;

    std::size_t const m_num_channels;
    stream_fn_t const m_stream_fn;

    audio_spsc_ring_buffer m_buffer;
    mipp::vector<float> m_interleave_buffer;
};

auto make_stream_processor(
        std::size_t num_channels,
        std::size_t capacity_per_channel,
        std::string_view const& name = {}) -> std::unique_ptr<stream_processor>;

} // namespace piejam::audio::engine
