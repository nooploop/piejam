// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/internal.h>

#include <piejam/audio/types.h>
#include <piejam/boxed_vector.h>
#include <piejam/ladspa/plugin_descriptor.h>

#include <variant>

namespace piejam::runtime::fx
{

struct registry
{
    using item = std::variant<internal, ladspa::plugin_descriptor>;

    boxed_vector<item> entries;

    auto operator==(registry const&) const noexcept -> bool = default;
};

[[nodiscard]] inline auto
make_internal_fx_registry_entries()
{
    return std::vector<registry::item>{
            {internal::tool},
            {internal::filter},
            {internal::scope},
            {internal::spectrum}};
}

[[nodiscard]] inline auto
make_default_registry() -> registry
{
    return {.entries = make_internal_fx_registry_entries()};
}

[[nodiscard]] inline auto
find_ladspa_plugin_descriptor(
        fx::registry const& registry,
        ladspa::plugin_id_t id) -> ladspa::plugin_descriptor const*
{
    for (auto const& item : *registry.entries)
    {
        if (auto const* const pd =
                    std::get_if<ladspa::plugin_descriptor>(&item);
            pd != nullptr && pd->id == id)
        {
            return pd;
        }
    }

    return nullptr;
}

struct is_available_for_bus_type
{
    audio::bus_type bus_type;

    [[nodiscard]] auto operator()(internal) const -> bool
    {
        return true;
    }

    [[nodiscard]] auto operator()(ladspa::plugin_descriptor const& pd) const
            -> bool
    {
        switch (bus_type)
        {
            case audio::bus_type::mono:
                return pd.num_inputs == 1 && pd.num_outputs == 1;

            case audio::bus_type::stereo:
                return (pd.num_inputs == 1 && pd.num_outputs == 1) ||
                       (pd.num_inputs == 2 && pd.num_outputs == 2);

            default:
                return false;
        }
    }

    [[nodiscard]] auto operator()(registry::item const& item) const -> bool
    {
        return std::visit(*this, item);
    }
};

} // namespace piejam::runtime::fx
