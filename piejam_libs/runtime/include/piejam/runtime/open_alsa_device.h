// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/runtime/fwd.h>

#include <memory>

namespace piejam::runtime
{

auto open_alsa_device(state const&)
        -> std::unique_ptr<piejam::audio::device>;

} // namespace piejam::runtime
