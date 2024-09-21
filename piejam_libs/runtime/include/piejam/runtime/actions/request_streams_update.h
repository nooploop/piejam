// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/audio_engine_action.h>
#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <boost/container/flat_set.hpp>

namespace piejam::runtime::actions
{

struct request_streams_update final
    : ui::cloneable_action<request_streams_update, action>
    , visitable_audio_engine_action<request_streams_update>
{
    boost::container::flat_set<audio_stream_id> streams;
};

} // namespace piejam::runtime::actions
