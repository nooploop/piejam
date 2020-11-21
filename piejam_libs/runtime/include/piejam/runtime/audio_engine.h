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
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/stereo_level.h>
#include <piejam/thread/fwd.h>

#include <map>
#include <memory>
#include <span>
#include <vector>

namespace piejam::runtime
{

class audio_engine
{
public:
    class mixer_bus;

    audio_engine(
            std::span<thread::configuration const> const& wt_configs,
            audio::samplerate_t,
            unsigned num_device_input_channels,
            unsigned num_device_output_channels);
    ~audio_engine();

    void set_bool_parameter(bool_parameter_id, bool);
    void set_float_parameter(float_parameter_id, float);

    void set_input_solo(mixer::bus_id const&);

    auto get_input_level(std::size_t index) const noexcept -> stereo_level;
    auto get_output_level(std::size_t index) const noexcept -> stereo_level;

    void
    rebuild(mixer::state const&,
            bool_parameters const&,
            float_parameters const&);

    void operator()(
            range::table_view<float const> const& in_audio,
            range::table_view<float> const& out_audio) noexcept;

private:
    std::vector<thread::configuration> const m_wt_configs;
    audio::samplerate_t const m_samplerate;

    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace piejam::runtime
