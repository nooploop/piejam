// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/runtime/persistence/fx_preset.h>

#include <piejam/runtime/fx/parameter_assignment.h>

namespace piejam::runtime::fx
{

void
to_json(nlohmann::json& j, parameter_assignment const& p)
{
    struct fx_parameter_value_to_json
    {
        auto operator()(float x) const { return nlohmann::json{{"float", x}}; }
        auto operator()(int x) const { return nlohmann::json{{"int", x}}; }
        auto operator()(bool x) const { return nlohmann::json{{"bool", x}}; }
    };

    j = {{"key", p.key},
         {"value", std::visit(fx_parameter_value_to_json{}, p.value)}};
}

void
from_json(nlohmann::json const& j, parameter_assignment& p)
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
