// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/update_streams.h>

#include <piejam/runtime/state.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

auto
update_streams::reduce(state const& st) const -> state
{
    auto new_st = st;

    for (auto&& [id, buffer] : streams)
    {
        new_st.streams.update(id, [&buffer](audio_stream_buffer& stored) {
            BOOST_ASSERT(stored->num_channels() == buffer->num_channels());
            stored = buffer;
        });
    }

    return new_st;
}

} // namespace piejam::runtime::actions
