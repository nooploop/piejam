// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/types.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <memory>
#include <string_view>

namespace piejam::runtime::components
{

auto make_mixer_bus_input(
        mixer::bus const&,
        audio::bus_type,
        parameter_processor_factory& param_procs,
        std::string_view const& name = {})
        -> std::unique_ptr<audio::engine::component>;

auto make_mixer_bus_output(
        audio::samplerate_t const samplerate,
        mixer::bus_id,
        mixer::bus const&,
        parameter_processor_factory& param_procs,
        std::string_view const& name = {})
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::components
