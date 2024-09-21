// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/box.h>
#include <piejam/io_pair.h>

#include <filesystem>
#include <string>
#include <vector>

namespace piejam::audio
{

struct sound_card_descriptor
{
    std::string name;
    std::filesystem::path path;

    auto operator==(sound_card_descriptor const& other) const noexcept
            -> bool = default;
};

using io_sound_cards = io_pair<box<std::vector<sound_card_descriptor>>>;

} // namespace piejam::audio
