// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/device.h>
#include <piejam/audio/fwd.h>
#include <piejam/midi/fwd.h>
#include <piejam/redux/functors.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/thread/configuration.h>

#include <functional>
#include <memory>
#include <span>
#include <vector>

namespace piejam::runtime
{

class audio_engine_middleware
{

public:
    using get_pcm_io_descriptors_f =
            std::function<piejam::audio::pcm_io_descriptors()>;
    using get_hw_params_f = std::function<piejam::audio::pcm_hw_params(
            piejam::audio::pcm_descriptor const&)>;
    using device_factory_f =
            std::function<std::unique_ptr<piejam::audio::device>(state const&)>;

    audio_engine_middleware(
            thread::configuration const& audio_thread_config,
            std::span<thread::configuration const> const& wt_configs,
            get_pcm_io_descriptors_f,
            get_hw_params_f,
            device_factory_f,
            get_state_f,
            dispatch_f,
            next_f);
    audio_engine_middleware(audio_engine_middleware&&) noexcept = default;
    ~audio_engine_middleware();

    void operator()(action const&);

private:
    template <class Action>
    void process_device_action(Action const&);

    template <class Action>
    void process_engine_action(Action const&);

    template <class Parameter>
    void process_engine_action(actions::set_parameter_value<Parameter> const&);

    void refresh_midi_devices();

    void close_device();
    void open_device();
    void start_engine();

    void rebuild();

    thread::configuration m_audio_thread_config;
    std::vector<thread::configuration> m_wt_configs;

    get_pcm_io_descriptors_f m_get_pcm_io_descriptors;
    get_hw_params_f m_get_hw_params;
    device_factory_f m_device_factory;
    get_state_f m_get_state;
    dispatch_f m_dispatch;
    next_f m_next;

    std::unique_ptr<midi::midi_manager> m_midi_manager;
    std::unique_ptr<fx::ladspa_manager> m_ladspa_fx_manager;

    std::unique_ptr<audio_engine> m_engine;
    std::unique_ptr<piejam::audio::device> m_device{
            std::make_unique<piejam::audio::dummy_device>()};
};

} // namespace piejam::runtime
