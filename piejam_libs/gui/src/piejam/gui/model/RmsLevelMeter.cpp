// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/RmsLevelMeter.h>

#include <piejam/audio/dsp/mipp_iterator.h>
#include <piejam/audio/dsp/rms_level_meter.h>

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::gui::model
{

struct RmsLevelMeter::Impl
{
    Impl(audio::sample_rate sr)
        : sample_rate{sr}
        , level_meter{sr}
    {
    }

    audio::sample_rate sample_rate;
    audio::dsp::rms_level_meter<> level_meter;
};

RmsLevelMeter::RmsLevelMeter(audio::sample_rate sr)
    : m_impl{make_pimpl<Impl>(sr)}
{
}

void
RmsLevelMeter::setSampleRate(audio::sample_rate sr)
{
    if (sr.valid() && m_impl->sample_rate != sr)
    {
        m_impl->level_meter = audio::dsp::rms_level_meter<>{sr};
        setLevel(m_impl->level_meter.level());
    }
}

void
RmsLevelMeter::update(AudioStream const& stream)
{
    BOOST_ASSERT(stream.num_channels() == 1);

    auto [pre, main, post] =
            audio::dsp::mipp_range_from_unaligned(stream.samples());

    std::ranges::copy(pre, std::back_inserter(m_impl->level_meter));
    std::ranges::copy(main, std::back_inserter(m_impl->level_meter));
    std::ranges::copy(post, std::back_inserter(m_impl->level_meter));
    setLevel(m_impl->level_meter.level());
}

} // namespace piejam::gui::model
