// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/io_process_config.h>
#include <piejam/audio/sound_card_descriptor.h>
#include <piejam/audio/sound_card_hw_params.h>
#include <piejam/audio/sound_card_manager.h>

#include <gmock/gmock.h>

namespace piejam::runtime::test
{

struct sound_card_manager_mock : public audio::sound_card_manager
{
    MOCK_METHOD(audio::io_sound_cards, io_descriptors, ());
    MOCK_METHOD(
            audio::sound_card_hw_params,
            hw_params,
            (audio::sound_card_descriptor const&,
             audio::sample_rate const*,
             audio::period_size const*));

    MOCK_METHOD(
            std::unique_ptr<audio::io_process>,
            make_io_process,
            (audio::sound_card_descriptor const&,
             audio::sound_card_descriptor const&,
             audio::io_process_config const&));
};

} // namespace piejam::runtime::test
