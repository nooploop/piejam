// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>

#include <piejam/audio/types.h>
#include <piejam/boxed_vector.h>
#include <piejam/ladspa/plugin_descriptor.h>

#include <variant>

namespace piejam::runtime::fx
{

struct registry
{
    using item = std::variant<internal_id, ladspa::plugin_descriptor>;

    auto operator==(registry const&) const noexcept -> bool = default;

    static auto register_internal_module() -> internal_id
    {
        auto id = internal_id::generate();
        internal_modules().update([id](std::vector<item>& internal_modules) {
            internal_modules.emplace_back(id);
        });
        return id;
    }

    boxed_vector<item> entries{internal_modules()};

private:
    static auto internal_modules() -> boxed_vector<item>&
    {
        static boxed_vector<item> s_modules;
        return s_modules;
    }
};

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

    [[nodiscard]] auto operator()(internal_id) const -> bool
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
