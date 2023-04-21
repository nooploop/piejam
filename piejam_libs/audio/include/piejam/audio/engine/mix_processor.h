// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::engine
{

auto make_mix_processor(std::size_t num_inputs, std::string_view name = {})
        -> std::unique_ptr<processor>;

auto is_mix_processor(processor const&) noexcept -> bool;

} // namespace piejam::audio::engine
