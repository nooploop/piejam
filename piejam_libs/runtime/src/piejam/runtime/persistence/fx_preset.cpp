// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/fx_preset.h>

#include <piejam/runtime/fx/parameter_assignment.h>

#include <nlohmann/json.hpp>

namespace piejam::runtime::fx
{

void
to_json(nlohmann::json& j, parameter_value_assignment const& p)
{
    struct fx_parameter_value_to_json
    {
        auto operator()(float x) const
        {
            return nlohmann::json{{"float", x}};
        }

        auto operator()(int x) const
        {
            return nlohmann::json{{"int", x}};
        }

        auto operator()(bool x) const
        {
            return nlohmann::json{{"bool", x}};
        }
    };

    j = {{"key", p.key},
         {"value", std::visit(fx_parameter_value_to_json{}, p.value)}};
}

void
from_json(nlohmann::json const& j, parameter_value_assignment& p)
{
    j.at("key").get_to(p.key);

    auto const& v = j.at("value");
    if (v.contains("float"))
    {
        float x;
        v["float"].get_to(x);
        p.value = x;
    }
    else if (v.contains("int"))
    {
        int x;
        v["int"].get_to(x);
        p.value = x;
    }
    else if (v.contains("bool"))
    {
        bool x;
        v["bool"].get_to(x);
        p.value = x;
    }
    else
    {
        throw std::runtime_error("unknown fx parameter value type");
    }
}

} // namespace piejam::runtime::fx
