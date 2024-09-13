// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/boxed_string.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/external_audio_fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct add_external_audio_device final
    : ui::cloneable_action<add_external_audio_device, reducible_action>
    , visitable_engine_action<add_external_audio_device>
{
    io_direction direction{};
    audio::bus_type type{};

    void reduce(state&) const override;
};

struct remove_external_audio_device final
    : ui::cloneable_action<remove_external_audio_device, reducible_action>
    , visitable_engine_action<remove_external_audio_device>
{
    external_audio::device_id device_id{};

    void reduce(state&) const override;
};

struct set_external_audio_device_bus_channel final
    : ui::cloneable_action<
              set_external_audio_device_bus_channel,
              reducible_action>
    , visitable_engine_action<set_external_audio_device_bus_channel>
{
    external_audio::device_id device_id{};
    audio::bus_channel channel_selector{};
    std::size_t channel_index{};

    void reduce(state&) const override;
};

struct set_external_audio_device_name final
    : ui::cloneable_action<set_external_audio_device_name, reducible_action>
{
    external_audio::device_id device_id;
    boxed_string name;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
