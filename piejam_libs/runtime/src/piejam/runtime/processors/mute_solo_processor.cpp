// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/mute_solo_processor.h>

#include <piejam/audio/engine/event_converter_processor.h>
#include <piejam/entity_id.h>
#include <piejam/npos.h>

namespace piejam::runtime::processors
{

auto
make_mute_solo_processor(
        mixer::bus_id const solo_id,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::processor>
{
    static std::array s_input_names{
            std::string_view("mute"),
            std::string_view("solo_index")};
    return std::unique_ptr<audio::engine::processor>{
            new audio::engine::event_converter_processor(
                    [solo_id](bool mute, mixer::bus_id in_solo_id) -> float {
                        return in_solo_id == mixer::bus_id{}
                                       ? !mute
                                       : in_solo_id == solo_id;
                    },
                    std::span(s_input_names),
                    "gain",
                    name)};
}

} // namespace piejam::runtime::processors
