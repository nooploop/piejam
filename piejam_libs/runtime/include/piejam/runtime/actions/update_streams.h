// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/recorder_action.h>
#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::actions
{

struct update_streams final
    : ui::cloneable_action<update_streams, reducible_action>
    , visitable_recorder_action<update_streams>
{
    boost::container::flat_map<audio_stream_id, audio_stream_buffer> streams;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
