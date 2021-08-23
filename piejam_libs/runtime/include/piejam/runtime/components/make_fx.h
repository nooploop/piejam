// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/get_parameter_name.h>
#include <piejam/runtime/fx/ladspa_processor_factory.h>

#include <memory>
#include <string_view>

namespace piejam::runtime::components
{

auto
make_fx(fx::module const&,
        fx::get_parameter_name const&,
        fx::simple_ladspa_processor_factory const&,
        parameter_processor_factory&,
        processors::stream_processor_factory&,
        audio::sample_rate const&,
        std::string_view const& name = {})
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::components
