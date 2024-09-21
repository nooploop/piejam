// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>

namespace piejam::audio::components
{

auto
make_identity(std::size_t num_channels) -> std::unique_ptr<engine::component>;

} // namespace piejam::audio::components
