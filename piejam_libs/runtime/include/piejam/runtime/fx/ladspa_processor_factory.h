// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <functional>
#include <memory>

namespace piejam::runtime::fx
{

using simple_ladspa_processor_factory =
        std::function<std::unique_ptr<audio::engine::processor>(
                ladspa::instance_id)>;

} // namespace piejam::runtime::fx
