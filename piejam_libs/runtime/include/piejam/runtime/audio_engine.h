// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/audio/pair.h>
#include <piejam/audio/pcm_buffer_converter.h>
#include <piejam/audio/types.h>
#include <piejam/midi/fwd.h>
#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/ladspa_processor_factory.h>
#include <piejam/runtime/stereo_level.h>
#include <piejam/thread/fwd.h>

#include <map>
#include <memory>
#include <optional>
#include <span>

namespace piejam::runtime
{

class audio_engine
{
public:
    audio_engine(
            std::span<thread::worker> workers,
            audio::sample_rate,
            unsigned num_device_input_channels,
            unsigned num_device_output_channels);
    ~audio_engine();

    template <class P>
    void set_parameter_value(parameter::id_t<P>, typename P::value_type const&)
            const;

    template <class P>
    [[nodiscard]] auto get_parameter_update(parameter::id_t<P>) const
            -> std::optional<typename P::value_type>;

    [[nodiscard]] auto get_learned_midi() const
            -> std::optional<midi::external_event>;

    [[nodiscard]] auto get_stream(audio_stream_id) const -> audio_stream_buffer;

    [[nodiscard]] auto
    rebuild(state const&,
            fx::simple_ladspa_processor_factory const&,
            std::unique_ptr<midi::input_event_handler>) -> bool;

    void init_process(
            std::span<audio::pcm_input_buffer_converter const>,
            std::span<audio::pcm_output_buffer_converter const>);

    void process(std::size_t buffer_size) noexcept;

private:
    struct impl;
    std::unique_ptr<impl> const m_impl;
};

} // namespace piejam::runtime
