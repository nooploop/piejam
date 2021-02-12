// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/device_manager.h>
#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/audio/pcm_io_config.h>

#include <gmock/gmock.h>

namespace piejam::runtime::test
{

struct audio_device_manager_mock : public audio::device_manager
{
    MOCK_METHOD(audio::pcm_io_descriptors, io_descriptors, ());
    MOCK_METHOD(
            audio::pcm_hw_params,
            hw_params,
            (audio::pcm_descriptor const&, audio::samplerate_t const*));

    MOCK_METHOD(
            std::unique_ptr<audio::device>,
            make_device,
            (audio::pcm_descriptor const&,
             audio::pcm_descriptor const&,
             audio::pcm_io_config const&));
};

} // namespace piejam::runtime::test
