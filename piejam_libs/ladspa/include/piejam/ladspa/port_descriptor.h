// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <variant>

namespace piejam::ladspa
{

struct float_port
{
    float min{};
    float max{1.f};
    float default_value{};
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

} // namespace piejam::ladspa
