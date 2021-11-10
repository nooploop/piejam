// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/mute_solo_processor.h>

#include <piejam/audio/engine/event_converter_processor.h>

namespace piejam::runtime::processors
{

auto
make_mute_solo_processor(std::string_view const name)
        -> std::unique_ptr<audio::engine::processor>
{
    static constexpr std::array const s_input_names{
            std::string_view("mute"),
            std::string_view("muted_by_solo")};
    return std::unique_ptr<audio::engine::processor>{
            new audio::engine::event_converter_processor(
                    [](bool mute, bool muted_by_solo) -> float {
                        return !(mute || muted_by_solo);
                    },
                    s_input_names,
                    "gain",
                    name)};
}

} // namespace piejam::runtime::processors
