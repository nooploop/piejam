// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/device_io_fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/stereo_level.h>

#include <piejam/audio/types.h>
#include <piejam/boxed_string.h>
#include <piejam/entity_map.h>

#include <boost/assert.hpp>

#include <vector>

namespace piejam::runtime::mixer
{

struct channel
{
    boxed_string name{};

    audio::bus_type bus_type{};

    io_address_t in{};
    io_address_t out{};

    float_parameter_id volume;
    float_parameter_id pan_balance;
    bool_parameter_id record;
    bool_parameter_id mute;
    bool_parameter_id solo;
    stereo_level_parameter_id peak_level;
    stereo_level_parameter_id rms_level;

    box<fx::chain_t> fx_chain{};

    auto operator==(channel const&) const noexcept -> bool = default;

    auto get_io_addr(io_socket const s) const noexcept -> io_address_t const&
    {
        switch (s)
        {
            case io_socket::in:
                return in;

            case io_socket::out:
                return out;
        }

        BOOST_ASSERT(false);
        static io_address_t const s_default;
        return s_default;
    }
};

struct state
{
    channels_t channels;

    box<channel_ids_t> inputs;
    channel_id main;
};

auto is_default_source_valid(channels_t const&, channel_id) -> bool;

auto valid_channels(io_socket, channels_t const&, channel_id)
        -> std::vector<channel_id>;

} // namespace piejam::runtime::mixer
