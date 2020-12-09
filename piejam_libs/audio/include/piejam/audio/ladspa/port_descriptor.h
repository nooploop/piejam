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

#pragma once

#include <string>
#include <variant>

namespace piejam::audio::ladspa
{

struct float_port
{
    float min{};
    float max{1.f};
    float default_value{};
    bool multiply_with_samplerate{};
    bool logarithmic{};
};

struct int_port
{
    int min{};
    int max{};
    int default_value{};
    bool logarithmic{};
};

struct bool_port
{
    bool default_value{};
};

using port_type_descriptor = std::variant<float_port, int_port, bool_port>;

struct port_descriptor
{
    unsigned long index{};
    std::string name;
    port_type_descriptor type_desc;
};

} // namespace piejam::audio::ladspa
