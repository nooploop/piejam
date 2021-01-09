// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/alsa/get_pcm_io_descriptors.h>
#include <piejam/audio/alsa/get_set_hw_params.h>
#include <piejam/runtime/actions/initiate_device_selection.h>
#include <piejam/runtime/audio_engine_middleware.h>
#include <piejam/runtime/open_alsa_device.h>
#include <piejam/runtime/state.h>
#include <piejam/thread/configuration.h>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <thread>

auto
main(int argc, char* argv[]) -> int
{
    if (argc != 6)
    {
        std::cout << fmt::format(
                             "Usage: {} <input> <output> <samplerate> "
                             "<period_size> <affinity>",
                             argv[0])
                  << std::endl;
        return -1;
    }

    try
    {
        piejam::runtime::state audio_state;
        audio_state.pcm_devices = piejam::audio::alsa::get_pcm_io_descriptors();
        audio_state.samplerate = boost::lexical_cast<unsigned>(argv[3]);
        audio_state.period_size = boost::lexical_cast<unsigned>(argv[4]);
        piejam::runtime::audio_engine_middleware audio_engine(
                piejam::thread::configuration{
                        .affinity = boost::lexical_cast<int>(argv[5]),
                        .priority = 96},
                {},
                &piejam::audio::alsa::get_pcm_io_descriptors,
                &piejam::audio::alsa::get_hw_params,
                &piejam::runtime::open_alsa_device,
                [&audio_state]() -> piejam::runtime::state const& {
                    return audio_state;
                },
                [&audio_state](piejam::runtime::action const& action) {
                    audio_state = action.reduce(audio_state);
                });

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
