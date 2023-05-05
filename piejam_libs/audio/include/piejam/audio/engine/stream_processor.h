// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/stream_ring_buffer.h>

#include <mipp.h>

#include <memory>
#include <string_view>

namespace piejam::audio::engine
{

class stream_processor final : public named_processor
{
public:
    stream_processor(
            std::size_t num_channels,
            std::size_t capacity_per_channel,
            std::string_view name = {});

    auto type_name() const noexcept -> std::string_view override
    {
        return "stream";
    }

    auto num_inputs() const noexcept -> std::size_t override
    {
        return m_num_channels;
    }

    auto num_outputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    auto event_inputs() const noexcept -> event_ports override
    {
        return {};
    }

    auto event_outputs() const noexcept -> event_ports override
    {
        return {};
    }

    void process(process_context const& ctx) override;

    auto consume()
    {
        return m_buffer.consume();
    }

private:
    std::size_t const m_num_channels;

    stream_ring_buffer m_buffer;
};

auto make_stream_processor(
        std::size_t num_channels,
        std::size_t capacity_per_channel,
        std::string_view name = {}) -> std::unique_ptr<stream_processor>;

} // namespace piejam::audio::engine
