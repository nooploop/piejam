// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/update_streams.h>

#include <piejam/runtime/state.h>

#include <piejam/audio/multichannel_buffer.h>

#include <boost/assert.hpp>

namespace piejam::runtime::actions
{

void
update_streams::reduce(state& st) const
{
    for (auto&& [id, buffer] : streams)
    {
        st.streams.update(id, [&buffer](audio_stream_buffer& stored) {
            BOOST_ASSERT(stored->num_channels() == buffer->num_channels());
            stored = buffer;
        });
    }
}

} // namespace piejam::runtime::actions
