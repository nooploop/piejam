// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/StreamChannelsBisector.h>

#include <piejam/audio/multichannel_buffer.h>

namespace piejam::gui::model
{

void
StreamChannelsBisector::update(Stream const& input)
{
    BOOST_ASSERT(input.num_channels() % 2 == 0);

    auto const num_channels = input.num_channels() / 2;
    audio::multichannel_buffer<float> first(
            input.layout(),
            num_channels,
            input.num_frames());
    audio::multichannel_buffer<float> second(
            input.layout(),
            num_channels,
            input.num_frames());

    auto itF = first.frames().begin();
    auto itS = second.frames().begin();
    for (auto itIn = input.frames().begin(); itIn != input.frames().end();
         ++itIn, ++itF, ++itS)
    {
        BOOST_ASSERT(itF != first.frames().end());
        BOOST_ASSERT(itS != second.frames().end());

        auto itInMid = std::next(itIn->begin(), num_channels);
        std::copy(itIn->begin(), itInMid, itF->begin());
        std::copy(itInMid, itIn->end(), itS->begin());
    }

    BOOST_ASSERT(itF == first.frames().end());
    BOOST_ASSERT(itS == second.frames().end());

    bisected(std::as_const(first).view(), std::as_const(second).view());
}

} // namespace piejam::gui::model
