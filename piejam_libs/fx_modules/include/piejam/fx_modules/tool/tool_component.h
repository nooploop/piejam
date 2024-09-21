// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::fx_modules::tool
{

auto make_component(runtime::internal_fx_component_factory_args const&)
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::fx_modules::tool
