// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/sound_card_hw_params.h>
#include <piejam/box.h>
#include <piejam/npos.h>

namespace piejam::runtime
{

struct selected_sound_card
{
    std::size_t index{npos};
    unique_box<audio::sound_card_hw_params> hw_params;

    auto operator==(selected_sound_card const&) const noexcept
            -> bool = default;
};

} // namespace piejam::runtime
