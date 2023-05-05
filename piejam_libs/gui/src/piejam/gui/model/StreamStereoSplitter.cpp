// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/StreamStereoSplitter.h>

#include <piejam/audio/multichannel_buffer.h>

namespace piejam::gui::model
{

void
StreamStereoSplitter::update(Stream const& input)
{
    std::size_t const num_channels = input.num_channels();

    BOOST_ASSERT(input.layout() == audio::multichannel_layout::non_interleaved);
    BOOST_ASSERT(num_channels <= 4);
    BOOST_ASSERT(num_channels % 2 == 0);

    std::vector<Stream> result;
    for (std::size_t ch = 0; ch < num_channels; ch += 2)
    {
        result.emplace_back(
                std::span{input.channels()[ch].data(), input.num_frames() * 2},
                2);
    }

    splitted(std::move(result));
}

} // namespace piejam::gui::model
