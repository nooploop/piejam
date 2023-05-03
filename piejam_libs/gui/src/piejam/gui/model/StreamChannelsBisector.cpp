// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/StreamChannelsBisector.h>

#include <piejam/audio/interleaved_vector.h>
#include <piejam/audio/interleaved_view.h>

namespace piejam::gui::model
{

void
StreamChannelsBisector::update(Stream const& input)
{
    BOOST_ASSERT(input.num_channels() % 2 == 0);

    auto const num_channels = input.num_channels() / 2;
    auto const vector_size = num_channels * input.num_frames();
    audio::interleaved_vector<float> first(
            std::vector<float>(vector_size),
            num_channels);
    audio::interleaved_vector<float> second(
            std::vector<float>(vector_size),
            num_channels);

    auto itF = first.begin();
    auto itS = second.begin();
    for (auto itIn = input.begin(); itIn != input.end(); ++itIn, ++itF, ++itS)
    {
        BOOST_ASSERT(itF != first.end());
        BOOST_ASSERT(itS != second.end());

        auto itInMid = std::next(itIn->begin(), num_channels);
        std::copy(itIn->begin(), itInMid, itF->begin());
        std::copy(itInMid, itIn->end(), itS->begin());
    }

    BOOST_ASSERT(itF == first.end());
    BOOST_ASSERT(itS == second.end());

    bisected(first.frames(), second.frames());
}

} // namespace piejam::gui::model
