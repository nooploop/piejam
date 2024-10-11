// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/PeakLevelMeter.h>

#include <piejam/audio/dsp/peak_level_meter.h>

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::gui::model
{

struct PeakLevelMeter::Impl
{
    Impl(audio::sample_rate sr)
        : sample_rate{sr}
        , level_meter{sr}
    {
    }

    audio::sample_rate sample_rate;
    audio::dsp::peak_level_meter<> level_meter;
};

PeakLevelMeter::PeakLevelMeter(audio::sample_rate sr)
    : m_impl{make_pimpl<Impl>(sr)}
{
}

void
PeakLevelMeter::setSampleRate(audio::sample_rate sr)
{
    if (sr.valid() && m_impl->sample_rate != sr)
    {
        m_impl->level_meter = audio::dsp::peak_level_meter<>{sr};
        setLevel(m_impl->level_meter.level());
    }
}

void
PeakLevelMeter::update(AudioStream const& stream)
{
    BOOST_ASSERT(stream.num_channels() == 1);

    std::ranges::copy(
            stream.samples(),
            std::back_inserter(m_impl->level_meter));
    setLevel(m_impl->level_meter.level());
}

} // namespace piejam::gui::model
