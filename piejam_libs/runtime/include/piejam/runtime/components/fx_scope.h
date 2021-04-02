// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/types.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::runtime::components
{

auto make_fx_scope(
        fx::module const&,
        audio::samplerate_t,
        processors::stream_processor_factory&,
        std::string_view const& name = {})
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::components
