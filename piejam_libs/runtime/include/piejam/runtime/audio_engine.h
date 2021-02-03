// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/audio/types.h>
#include <piejam/midi/fwd.h>
#include <piejam/range/table_view.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/ladspa_processor_factory.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameter_maps.h>
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
            std::span<thread::configuration const> const& wt_configs,
            audio::samplerate_t,
            unsigned num_device_input_channels,
            unsigned num_device_output_channels);
    ~audio_engine();

    template <class P>
    void set_parameter_value(parameter::id_t<P>, typename P::value_type const&)
            const;

    template <class P>
    auto get_parameter_update(parameter::id_t<P>) const
            -> std::optional<typename P::value_type>;

    void set_input_solo(mixer::bus_id const&);

    auto get_learned_midi() const -> std::optional<midi::external_event>;

    [[nodiscard]] bool
    rebuild(mixer::state const&,
            fx::modules_t const&,
            fx::parameters_t const& fx_params,
            parameter_maps const&,
            fx::ladspa_processor_factory const&,
            std::unique_ptr<midi::input_processor>,
            bool midi_learn,
            midi_assignments_map const&);

    void operator()(
            range::table_view<float const> const& in_audio,
            range::table_view<float> const& out_audio) noexcept;

private:
    audio::samplerate_t const m_samplerate;

    struct impl;
    std::unique_ptr<impl> const m_impl;
};

} // namespace piejam::runtime
