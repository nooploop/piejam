// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_hw_params.h>
#include <piejam/box.h>
#include <piejam/npos.h>

namespace piejam::runtime
{

struct selected_device
{
    std::size_t index{npos};
    box<audio::pcm_hw_params> hw_params;

    auto operator==(selected_device const&) const noexcept -> bool = default;
};

} // namespace piejam::runtime
