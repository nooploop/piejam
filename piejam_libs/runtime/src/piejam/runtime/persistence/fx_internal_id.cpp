// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/fx_internal_id.h>

#include <piejam/entity_id.h>

#include <nlohmann/json.hpp>

#include <boost/assert.hpp>
#include <boost/bimap/bimap.hpp>

namespace
{

using internal_fx_names_map =
        boost::bimaps::bimap<piejam::runtime::fx::internal_id, std::string>;

auto
internal_fx_names() -> internal_fx_names_map&
{
    static internal_fx_names_map s_map;
    return s_map;
}

} // namespace

namespace piejam::runtime::fx
{

void
to_json(nlohmann::json& j, internal_id const& id)
{
    auto it = internal_fx_names().left.find(id);
    BOOST_ASSERT(it != internal_fx_names().left.end());

    j = it->second;
}

void
from_json(nlohmann::json const& j, internal_id& id)
{
    std::string name;
    j.get_to<std::string>(name);

    auto it = internal_fx_names().right.find(name);
    if (it != internal_fx_names().right.end())
    {
        id = it->second;
    }
}

} // namespace piejam::runtime::fx

namespace piejam::runtime::persistence
{

void
register_internal_fx(fx::internal_id id, std::string name)
{
    using value_type = typename internal_fx_names_map::value_type;
    BOOST_VERIFY(internal_fx_names()
                         .insert(value_type{std::move(id), std::move(name)})
                         .second);
}

} // namespace piejam::runtime::persistence
