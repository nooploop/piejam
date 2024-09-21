// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/get_parameter_name.h>

#include <functional>
#include <memory>

namespace piejam::runtime::ladspa_fx
{

using processor_factory =
        std::function<std::unique_ptr<audio::engine::processor>()>;

auto make_component(
        fx::module const&,
        fx::get_parameter_name const&,
        processor_factory const&,
        parameter_processor_factory&)
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::ladspa_fx
