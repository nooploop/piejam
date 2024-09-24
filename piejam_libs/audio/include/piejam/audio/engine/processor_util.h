// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/processor.h>

namespace piejam::audio::engine
{

[[nodiscard]]
inline auto
is_mono_in_out_processor(processor const& p) -> bool
{
    return p.num_inputs() == 1 && p.num_outputs() == 1;
}

[[nodiscard]]
inline auto
is_stereo_in_out_processor(processor const& p) -> bool
{
    return p.num_inputs() == 2 && p.num_outputs() == 2;
}

} // namespace piejam::audio::engine
