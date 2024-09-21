// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <nlohmann/json.hpp>

#include <string_view>

namespace piejam::runtime::persistence
{

template <class StrongType, class UnderlyingType>
struct strong_type_serializer
{
    std::string_view value_key;

    void to_json(nlohmann::json& j, StrongType const& x) const
    {
        j = nlohmann::json{{value_key, x.value()}};
    }

    void from_json(nlohmann::json const& j, StrongType& x) const
    {
        x = StrongType{j.at(value_key).get<UnderlyingType>()};
    }
};

} // namespace piejam::runtime::persistence

#define M_PIEJAM_PERSISTENCE_DEFINE_STRONG_TYPE_SERIALIER(                     \
        StrongType,                                                            \
        UnderlyingType,                                                        \
        ValueKey)                                                              \
    static piejam::runtime::persistence::strong_type_serializer<               \
            StrongType,                                                        \
            UnderlyingType> const s_##StrongType{ValueKey};                    \
                                                                               \
    void to_json(nlohmann::json& j, StrongType const& x)                       \
    {                                                                          \
        s_##StrongType.to_json(j, x);                                          \
    }                                                                          \
                                                                               \
    void from_json(nlohmann::json const& j, StrongType& x)                     \
    {                                                                          \
        s_##StrongType.from_json(j, x);                                        \
    }
