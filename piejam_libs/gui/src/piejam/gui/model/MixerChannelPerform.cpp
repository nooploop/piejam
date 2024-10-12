// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelPerform.h>

#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/StereoLevel.h>

#include <piejam/audio/dsp/mipp_iterator.h>
#include <piejam/audio/dsp/peak_level_meter.h>
#include <piejam/audio/dsp/rms_level_meter.h>
#include <piejam/audio/pair.h>
#include <piejam/renew.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct MixerChannelPerform::Impl
{
    static constexpr audio::sample_rate default_sample_rate{48000u};
    audio::sample_rate sample_rate{default_sample_rate};

    StereoLevel peakLevel;
    StereoLevel rmsLevel;
    std::unique_ptr<FxStream> outStream;

    audio::pair<audio::dsp::peak_level_meter<>> peak_level_meter{
            default_sample_rate};
    audio::pair<audio::dsp::rms_level_meter<>> rms_level_meter{
            default_sample_rate};

    std::unique_ptr<FloatParameter> volume;
    std::unique_ptr<FloatParameter> panBalance;
    std::unique_ptr<BoolParameter> record;
    std::unique_ptr<BoolParameter> solo;
    std::unique_ptr<BoolParameter> mute;

    void updateSampleRate(audio::sample_rate sr)
    {
        if (sr.valid() && sample_rate != sr)
        {
            sample_rate = sr;
            renew(peak_level_meter, sr);
            renew(rms_level_meter, sr);
        }
    }

    template <std::size_t I>
    void calcPeakLevel(std::span<float const> ch)
    {
        std::ranges::copy(ch, std::back_inserter(get<I>(peak_level_meter)));
        peakLevel.setLevel<I>(get<I>(peak_level_meter).level());
    }

    template <std::size_t I>
    void calcRmsLevel(std::span<float const> ch)
    {
        auto [pre, main, post] = audio::dsp::mipp_range_from_unaligned(ch);

        std::ranges::copy(pre, std::back_inserter(get<I>(rms_level_meter)));
        std::ranges::copy(main, std::back_inserter(get<I>(rms_level_meter)));
        std::ranges::copy(post, std::back_inserter(get<I>(rms_level_meter)));

        rmsLevel.setLevel<I>(get<I>(rms_level_meter).level());
    }
};

MixerChannelPerform::MixerChannelPerform(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id)
    : MixerChannel{store_dispatch, state_change_subscriber, id}
    , m_impl{make_pimpl<Impl>()}
{
    m_impl->outStream = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            observe_once(
                    runtime::selectors::make_mixer_channel_out_stream_selector(
                            id)));
    MixerChannel::connectSubscribableChild(*m_impl->outStream);

    QObject::connect(
            m_impl->outStream.get(),
            &AudioStreamProvider::captured,
            this,
            [this](AudioStream captured) {
                auto captured_stereo = captured.channels_cast<2>();

                m_impl->calcPeakLevel<0>(captured_stereo.channels()[0]);
                m_impl->calcPeakLevel<1>(captured_stereo.channels()[1]);

                m_impl->calcRmsLevel<0>(captured_stereo.channels()[0]);
                m_impl->calcRmsLevel<1>(captured_stereo.channels()[1]);
            });

    makeParameter(
            m_impl->volume,
            observe_once(
                    runtime::selectors::
                            make_mixer_channel_volume_parameter_selector(id)));

    makeParameter(
            m_impl->panBalance,
            observe_once(
                    runtime::selectors::
                            make_mixer_channel_pan_balance_parameter_selector(
                                    id)));

    makeParameter(
            m_impl->record,
            observe_once(
                    runtime::selectors::
                            make_mixer_channel_record_parameter_selector(id)));

    makeParameter(
            m_impl->solo,
            observe_once(
                    runtime::selectors::
                            make_mixer_channel_solo_parameter_selector(id)));

    makeParameter(
            m_impl->mute,
            observe_once(
                    runtime::selectors::
                            make_mixer_channel_mute_parameter_selector(id)));
}

auto
MixerChannelPerform::peakLevel() const noexcept -> StereoLevel*
{
    return &m_impl->peakLevel;
}

auto
MixerChannelPerform::rmsLevel() const noexcept -> StereoLevel*
{
    return &m_impl->rmsLevel;
}

auto
MixerChannelPerform::volume() const noexcept -> FloatParameter*
{
    return m_impl->volume.get();
}

auto
MixerChannelPerform::panBalance() const noexcept -> FloatParameter*
{
    return m_impl->panBalance.get();
}

auto
MixerChannelPerform::record() const noexcept -> BoolParameter*
{
    return m_impl->record.get();
}

auto
MixerChannelPerform::solo() const noexcept -> BoolParameter*
{
    return m_impl->solo.get();
}

auto
MixerChannelPerform::mute() const noexcept -> BoolParameter*
{
    return m_impl->mute.get();
}

void
MixerChannelPerform::onSubscribe()
{
    MixerChannel::onSubscribe();

    m_impl->updateSampleRate(
            observe_once(runtime::selectors::select_sample_rate)->current);

    observe(runtime::selectors::make_muted_by_solo_selector(channel_id()),
            [this](bool x) { setMutedBySolo(x); });
}

} // namespace piejam::gui::model
