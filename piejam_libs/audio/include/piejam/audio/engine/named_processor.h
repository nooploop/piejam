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

#include <piejam/audio/engine/processor.h>

#include <string>

namespace piejam::audio::engine
{

class named_processor : public processor
{
public:
    named_processor(std::string_view const& name = {})
        : m_name(name)
    {
    }

    auto name() const -> std::string_view override { return m_name; }

private:
    std::string const m_name;
};

} // namespace piejam::audio::engine
