// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::engine
{

auto make_identity_processor(std::string_view const& name = {})
        -> std::unique_ptr<processor>;

bool is_identity_processor(processor const&);

} // namespace piejam::audio::engine
