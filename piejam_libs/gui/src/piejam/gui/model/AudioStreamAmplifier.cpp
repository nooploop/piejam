// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioStreamAmplifier.h>

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/functional/operators.h>
#include <piejam/range/indices.h>

namespace piejam::gui::model
{

AudioStreamAmplifier::AudioStreamAmplifier(std::size_t numSubstreams)
    : m_gains(numSubstreams, 1.f)
{
}

void
AudioStreamAmplifier::update(AudioStream const& stream)
{
    // fast path if all gains are at unity
    if (std::ranges::all_of(m_gains, equal_to<>(1.f)))
    {
        amplified(stream);
        return;
    }

    using Buffer = audio::multichannel_buffer<
            float,
            audio::multichannel_layout_non_interleaved>;

    Buffer buffer{stream.num_channels(), stream.num_frames()};

    std::size_t const channelsPerSubstream =
            stream.num_channels() / m_gains.size();

    for (std::size_t const substreamIndex : range::indices(m_gains))
    {
        auto srcSubstream = stream.channels_subview(
                substreamIndex * channelsPerSubstream,
                channelsPerSubstream);

        auto dstSubstream = buffer.view().channels_subview(
                substreamIndex * channelsPerSubstream,
                channelsPerSubstream);

        if (float const gain = m_gains[substreamIndex]; gain != 1.f)
        {
            transform(
                    srcSubstream.channels(),
                    dstSubstream.channels(),
                    [gain = m_gains[substreamIndex]](auto const sample) {
                        return gain * sample;
                    });
        }
        else
        {
            copy(srcSubstream.channels(), dstSubstream.channels());
        }
    }

    amplified(std::as_const(buffer).view());
}

void
AudioStreamAmplifier::setGain(std::size_t substreamIndex, float gain)
{
    m_gains[substreamIndex] = gain;
}

} // namespace piejam::gui::model
