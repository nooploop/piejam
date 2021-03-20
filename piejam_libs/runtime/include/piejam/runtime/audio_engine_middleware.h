// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/ladspa_processor_factory.h>
#include <piejam/runtime/middleware_functors.h>
#include <piejam/thread/configuration.h>
#include <piejam/thread/fwd.h>

#include <functional>
#include <memory>
#include <span>
#include <vector>

namespace piejam::runtime
{

class audio_engine_middleware final : private middleware_functors
{
public:
    audio_engine_middleware(
            middleware_functors,
            thread::configuration const& audio_thread_config,
            std::span<thread::configuration const> const& wt_configs,
            audio::device_manager&,
            fx::ladspa_processor_factory,
            std::unique_ptr<midi_input_controller>);
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

    void close_device();
    void open_device();
    void start_engine();

    void rebuild();

    thread::configuration m_audio_thread_config;
    std::vector<thread::worker> m_workers;

    audio::device_manager& m_device_manager;
    fx::ladspa_processor_factory m_ladspa_fx_processor_factory;
    std::unique_ptr<midi_input_controller> m_midi_controller;

    std::unique_ptr<audio_engine> m_engine;
    std::unique_ptr<audio::device> m_device;
};

} // namespace piejam::runtime
