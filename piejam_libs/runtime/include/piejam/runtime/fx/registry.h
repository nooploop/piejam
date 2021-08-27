// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/boxed_vector.h>
#include <piejam/runtime/fx/internal.h>

#include <variant>

namespace piejam::runtime::fx
{

struct registry
{
    using item = std::variant<internal, audio::ladspa::plugin_descriptor>;

    boxed_vector<item> entries;

    bool operator==(registry const&) const noexcept = default;
};

inline auto
make_internal_fx_registry_entries()
{
    return std::vector<registry::item>{
            {internal::gain},
            {internal::filter},
            {internal::scope},
            {internal::spectrum}};
}

inline auto
make_default_registry() -> registry
{
    return {.entries = make_internal_fx_registry_entries()};
}

inline auto
find_ladspa_plugin_descriptor(
        fx::registry const& registry,
        audio::ladspa::plugin_id_t id)
        -> audio::ladspa::plugin_descriptor const*
{
    for (auto const& item : *registry.entries)
    {
        if (auto pd = std::get_if<audio::ladspa::plugin_descriptor>(&item);
            pd && pd->id == id)
            return pd;
    }

    return nullptr;
}

} // namespace piejam::runtime::fx
