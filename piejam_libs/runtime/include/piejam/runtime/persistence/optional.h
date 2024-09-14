// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <nlohmann/json.hpp>

#include <optional>
#include <string_view>

namespace piejam::runtime::persistence
{

template <class T>
void
optional_to_json(
        nlohmann::json& j,
        std::optional<T> const& value,
        std::string_view const name)
{
    if (value)
    {
        j[name] = *value;
    }
}

template <class T>
void
optional_from_json(
        nlohmann::json const& j,
        std::optional<T>& value,
        std::string_view const name)
{
    if (j.contains(name))
    {
        value = j.at(name).get<T>();
    }
}

template <class T>
struct optional_serializer
{
    std::string_view name;

    void to_json(nlohmann::json& j, std::optional<T> const& value) const
    {
        optional_to_json(j, value, name);
    }

    void from_json(nlohmann::json const& j, std::optional<T>& value) const
    {
        optional_from_json(j, value, name);
    }
};

} // namespace piejam::runtime::persistence
