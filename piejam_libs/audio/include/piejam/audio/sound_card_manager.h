// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/audio/sound_card_descriptor.h>

#include <memory>

namespace piejam::audio
{

class sound_card_manager
{
public:
    virtual ~sound_card_manager() = default;

    virtual auto io_descriptors() -> io_sound_cards = 0;

    virtual auto hw_params(
            sound_card_descriptor const&,
            sample_rate const*,
            period_size const*) -> sound_card_hw_params = 0;

    virtual auto make_io_process(
            sound_card_descriptor const& in,
            sound_card_descriptor const& out,
            io_process_config const&) -> std::unique_ptr<io_process> = 0;
};

auto make_sound_card_manager() -> std::unique_ptr<sound_card_manager>;

} // namespace piejam::audio
