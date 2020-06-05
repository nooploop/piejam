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

#include <piejam/audio/device.h>
#include <piejam/audio/fwd.h>
#include <piejam/audio/mixer.h>
#include <piejam/redux/functors.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/fwd.h>

#include <functional>
#include <memory>

namespace piejam::runtime
{

class audio_engine_middleware
{

public:
    using get_state_f = redux::get_state_f<audio_state>;
    using next_f = redux::next_f<action>;
    using get_pcm_io_descriptors_f =
            std::function<piejam::audio::pcm_io_descriptors()>;
    using get_hw_params_f = std::function<piejam::audio::pcm_hw_params(
            piejam::audio::pcm_descriptor const&)>;
    using device_factory_f =
            std::function<std::unique_ptr<piejam::audio::device>(
                    audio_state const&)>;

    audio_engine_middleware(
            int audio_thread_cpu_affinity,
            get_pcm_io_descriptors_f,
            get_hw_params_f,
            device_factory_f,
            get_state_f,
            next_f);
    audio_engine_middleware(audio_engine_middleware&&) noexcept = default;
    ~audio_engine_middleware();

    void operator()(action const& action);

private:
    void process_device_action(actions::device_action const&);
    void process_engine_action(actions::engine_action const&);

    void process_apply_app_config_action(actions::apply_app_config const&);
    void process_refresh_devices_action(actions::refresh_devices const&);
    void process_initiate_device_selection_action(
            actions::initiate_device_selection const&);

    void close_device();
    void open_device();
    void start_engine();

    int m_audio_thread_cpu_affinity{};

    get_pcm_io_descriptors_f m_get_pcm_io_descriptors;
    get_hw_params_f m_get_hw_params;
    device_factory_f m_device_factory;
    get_state_f m_get_state;
    next_f m_next;

    std::unique_ptr<piejam::audio::engine> m_engine;
    std::unique_ptr<piejam::audio::device> m_device{
            std::make_unique<piejam::audio::dummy_device>()};
};

} // namespace piejam::runtime
