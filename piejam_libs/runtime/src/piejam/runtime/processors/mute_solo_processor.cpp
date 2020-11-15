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

#include <piejam/runtime/processors/mute_solo_processor.h>

#include <piejam/audio/engine/event_converter_processor.h>
#include <piejam/npos.h>

namespace piejam::runtime::processors
{

auto
make_mute_solo_processor(
        mixer::bus_id const& solo_id,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::processor>
{
    static std::array s_input_names{
            std::string_view("mute"),
            std::string_view("solo_index")};
    return std::unique_ptr<audio::engine::processor>{
            new audio::engine::event_converter_processor(
                    [solo_id](bool mute, mixer::bus_id in_solo_id)
                            -> float {
                        return in_solo_id == mixer::bus_id{}
                                       ? !mute
                                       : in_solo_id == solo_id;
                    },
                    std::span(s_input_names),
                    "gain",
                    name)};
}

} // namespace piejam::runtime::processors
