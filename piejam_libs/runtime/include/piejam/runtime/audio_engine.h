// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/audio/types.h>
#include <piejam/range/table_view.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/ladspa_processor_factory.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameter_maps.h>
#include <piejam/runtime/stereo_level.h>
#include <piejam/thread/fwd.h>

#include <map>
#include <memory>
#include <optional>
#include <span>
#include <vector>

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

    void set_bool_parameter(bool_parameter_id, bool) const;
    void set_float_parameter(float_parameter_id, float) const;
    void set_int_parameter(int_parameter_id, int) const;

    void set_input_solo(mixer::bus_id const&);

    auto get_level(stereo_level_parameter_id) const
            -> std::optional<stereo_level>;

    void
    rebuild(mixer::state const&,
            fx::modules_t const&,
            fx::parameters_t const& fx_params,
            parameter_maps const&,
            fx::ladspa_processor_factory const&);

    void operator()(
            range::table_view<float const> const& in_audio,
            range::table_view<float> const& out_audio) noexcept;

private:
    std::vector<thread::configuration> const m_wt_configs;
    audio::samplerate_t const m_samplerate;

    struct impl;
    std::unique_ptr<impl> const m_impl;
};

} // namespace piejam::runtime
