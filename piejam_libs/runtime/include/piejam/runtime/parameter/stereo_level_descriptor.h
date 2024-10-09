// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/boxed_string.h>

namespace piejam::runtime::parameter
{

struct stereo_level_descriptor
{
    using value_type = audio::pair<float>;

    boxed_string name;
};

} // namespace piejam::runtime::parameter
