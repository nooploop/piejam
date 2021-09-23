// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/device_manager.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/ladspa/instance_manager_processor_factory.h>
#include <piejam/ladspa/plugin.h>
#include <piejam/midi/device_manager.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/runtime/actions/initiate_device_selection.h>
#include <piejam/runtime/audio_engine_middleware.h>
#include <piejam/runtime/midi_input_controller.h>
#include <piejam/runtime/state.h>
#include <piejam/thread/configuration.h>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <memory>
#include <thread>

auto
main(int argc, char* argv[]) -> int
{
    if (argc != 6)
    {
        std::cout << fmt::format(
                             "Usage: {} <input> <output> <sample_rate> "
                             "<period_size> <affinity>",
                             argv[0])
                  << std::endl;
        return -1;
    }

    try
    {
        auto audio_device_manager = piejam::audio::make_device_manager();

        piejam::ladspa::instance_manager_processor_factory ladspa_manager;

        piejam::runtime::state audio_state;
        audio_state.pcm_devices = audio_device_manager->io_descriptors();
        audio_state.sample_rate = piejam::audio::sample_rate(
                boost::lexical_cast<unsigned>(argv[3]));
        audio_state.period_size = piejam::audio::period_size(
                boost::lexical_cast<unsigned>(argv[4]));
        piejam::runtime::audio_engine_middleware audio_engine(
                piejam::runtime::middleware_functors(
                        [&audio_state]() -> piejam::runtime::state const& {
                            return audio_state;
                        },
                        [&audio_state](piejam::runtime::action const& action) {
                            audio_state = action.reduce(audio_state);
                        },
                        [&audio_state](piejam::runtime::action const& action) {
                            audio_state = action.reduce(audio_state);
                        }),
                piejam::thread::configuration{
                        .affinity = boost::lexical_cast<int>(argv[5]),
                        .realtime_priority = 96},
                {},
                *audio_device_manager,
                ladspa_manager,
                nullptr);

        {
            piejam::runtime::actions::initiate_device_selection select_input;
            select_input.input = true;
            select_input.index = boost::lexical_cast<unsigned>(argv[1]);

            audio_engine(select_input);
        }

        {
            piejam::runtime::actions::initiate_device_selection select_output;
            select_output.input = false;
            select_output.index = boost::lexical_cast<unsigned>(argv[2]);

            audio_engine(select_output);
        }

        std::this_thread::sleep_for(std::chrono::seconds{100});
    }
    catch (std::exception const& e)
    {
        spdlog::error(e.what());
        return -1;
    }
    return 0;
}
