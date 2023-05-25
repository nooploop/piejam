// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/multichannel_view.h>

namespace piejam::gui::model
{

using AudioStream = audio::multichannel_view<
        float const,
        audio::multichannel_layout_non_interleaved>;

} // namespace piejam::gui::model
