// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::gui::model
{

struct FxStreamKeyId
{
    runtime::fx::stream_key key{};
    runtime::audio_stream_id id;

    constexpr bool
    operator==(FxStreamKeyId const& other) const noexcept = default;
};

} // namespace piejam::gui::model
