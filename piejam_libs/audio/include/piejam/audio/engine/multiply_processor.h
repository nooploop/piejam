// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::engine
{

auto make_multiply_processor(
        std::size_t num_inputs,
        std::string_view const& name = {}) -> std::unique_ptr<processor>;

} // namespace piejam::audio::engine
