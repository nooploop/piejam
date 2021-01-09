// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_hw_params.h>
#include <piejam/box.h>

namespace piejam::runtime
{

struct selected_device
{
    std::size_t index{};
    box<audio::pcm_hw_params> hw_params;
};

inline bool
operator==(selected_device const& l, selected_device const& r) noexcept
{
    return l.index == r.index && l.hw_params == r.hw_params;
}

} // namespace piejam::runtime
