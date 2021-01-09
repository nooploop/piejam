// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/fwd.h>

#include <functional>

namespace piejam::audio
{

using process_function = std::function<void(
        range::table_view<float const> const& /* in */,
        range::table_view<float> const& /* out */)>;

} // namespace piejam::audio
