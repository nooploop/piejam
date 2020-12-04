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

#include <piejam/runtime/fx/type.h>

#include <vector>

namespace piejam::runtime::fx
{

struct registry
{
    using item = type;

    std::vector<item> entries;
};

inline bool
operator==(registry const& l, registry const& r) noexcept
{
    return l.entries == r.entries;
}

inline bool
operator!=(registry const& l, registry const& r) noexcept
{
    return l.entries != r.entries;
}

inline auto
make_default_registry() -> registry
{
    return {.entries = {type::gain}};
}

} // namespace piejam::runtime::fx
