// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/boxed_string.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/unavailable_ladspa.h>

namespace piejam::runtime::fx
{

struct module
{
    instance_id fx_instance_id;
    boxed_string name;
    bool bypassed{};
    box<module_parameters> parameters;
    box<module_streams> streams;

    bool operator==(module const&) const noexcept = default;
};

} // namespace piejam::runtime::fx
