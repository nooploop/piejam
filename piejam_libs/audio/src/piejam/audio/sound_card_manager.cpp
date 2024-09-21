// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/sound_card_manager.h>

#include "alsa/get_io_sound_cards.h"
#include "alsa/get_set_hw_params.h"
#include "alsa/pcm_io.h"

#include <piejam/audio/sound_card_descriptor.h>
#include <piejam/audio/sound_card_hw_params.h>

#include <piejam/box.h>
#include <piejam/io_pair.h>

namespace piejam::audio
{

namespace
{

class alsa_sound_card_manager final : public sound_card_manager
{
public:
    auto io_descriptors() -> io_sound_cards override
    {
        return alsa::get_io_sound_cards();
    }

    auto hw_params(
            sound_card_descriptor const& d,
            sample_rate const* const sample_rate,
            period_size const* const period_size)
            -> sound_card_hw_params override
    {
        return alsa::get_hw_params(d, sample_rate, period_size);
    }

    auto make_io_process(
            sound_card_descriptor const& in,
            sound_card_descriptor const& out,
            io_process_config const& config)
            -> std::unique_ptr<io_process> override
    {
        return std::make_unique<alsa::pcm_io>(in, out, config);
    }
};

} // namespace

auto
make_sound_card_manager() -> std::unique_ptr<sound_card_manager>
{
    return std::make_unique<alsa_sound_card_manager>();
}

} // namespace piejam::audio
