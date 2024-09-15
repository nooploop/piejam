// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/unavailable_ladspa.h>

#include <piejam/audio/types.h>
#include <piejam/boxed_string.h>
#include <piejam/entity_id.h>

namespace piejam::runtime::fx
{

struct module
{
    instance_id fx_instance_id;
    boxed_string name;
    audio::bus_type bus_type{};
    bool bypassed{};
    unique_box<module_parameters> parameters;
    unique_box<module_streams> streams;

    auto operator==(module const&) const noexcept -> bool = default;
};

} // namespace piejam::runtime::fx
