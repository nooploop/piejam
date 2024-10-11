// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioStreamChannelSplitter.h>

#include <piejam/audio/multichannel_view.h>

namespace piejam::gui::model
{

AudioStreamChannelSplitter::AudioStreamChannelSplitter(
        std::span<BusType const> substreamConfigs)
    : m_substreamConfigs(substreamConfigs.begin(), substreamConfigs.end())
{
}

void
AudioStreamChannelSplitter::update(AudioStream const& stream)
{
    std::size_t substream_index{};
    std::size_t start_channel{};
    for (auto busType : m_substreamConfigs)
    {
        auto num_channels = audio::num_channels(toBusType(busType));

        emit splitted(
                substream_index,
                stream.channels_subview(start_channel, num_channels));

        ++substream_index;
        start_channel += num_channels;
    }
}

} // namespace piejam::gui::model
