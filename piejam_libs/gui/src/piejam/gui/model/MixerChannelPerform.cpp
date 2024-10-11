// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelPerform.h>

#include <piejam/gui/model/AudioStreamChannelSplitter.h>
#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/DbScaleData.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/gui/model/MixerDbScales.h>
#include <piejam/gui/model/PeakLevelMeter.h>
#include <piejam/gui/model/RmsLevelMeter.h>
#include <piejam/gui/model/StereoLevel.h>

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/selectors.h>

#include <array>

namespace piejam::gui::model
{

struct MixerChannelPerform::Impl
{
    StereoLevel peakLevel;
    StereoLevel rmsLevel;
    std::unique_ptr<FxStream> outStream;
    AudioStreamChannelSplitter stereoSplitter{
            std::array{BusType::Mono, BusType::Mono}};
    PeakLevelMeter peakLevelMeterLeft{audio::sample_rate{48000u}};
    PeakLevelMeter peakLevelMeterRight{audio::sample_rate{48000u}};
    RmsLevelMeter rmsLevelMeterLeft{audio::sample_rate{48000u}};
    RmsLevelMeter rmsLevelMeterRight{audio::sample_rate{48000u}};

    std::unique_ptr<FloatParameter> volume;
    std::unique_ptr<FloatParameter> panBalance;
    std::unique_ptr<BoolParameter> record;
    std::unique_ptr<BoolParameter> solo;
    std::unique_ptr<BoolParameter> mute;

    void updateSampleRate(audio::sample_rate sr)
    {
        peakLevelMeterLeft.setSampleRate(sr);
        peakLevelMeterRight.setSampleRate(sr);
        rmsLevelMeterLeft.setSampleRate(sr);
        rmsLevelMeterRight.setSampleRate(sr);
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

    m_impl->updateSampleRate(
            observe_once(runtime::selectors::select_sample_rate)->current);

    QObject::connect(
            m_impl->outStream.get(),
            &AudioStreamProvider::captured,
            &m_impl->stereoSplitter,
            &AudioStreamListener::update);

    QObject::connect(
            &m_impl->stereoSplitter,
            &AudioStreamChannelSplitter::splitted,
            this,
            [this](std::size_t substreamIndex, AudioStream stream) {
                switch (substreamIndex)
                {
                    case 0:
                        m_impl->peakLevelMeterLeft.update(stream);
                        m_impl->rmsLevelMeterLeft.update(stream);
                        break;

                    case 1:
                        m_impl->peakLevelMeterRight.update(stream);
                        m_impl->rmsLevelMeterRight.update(stream);
                        break;

                    default:
                        BOOST_ASSERT(false);
                }
            });

    QObject::connect(
            &m_impl->peakLevelMeterLeft,
            &PeakLevelMeter::levelChanged,
            this,
            [this]() {
                m_impl->peakLevel.setLevelLeft(
                        m_impl->peakLevelMeterLeft.level());
            });
    QObject::connect(
            &m_impl->peakLevelMeterRight,
            &PeakLevelMeter::levelChanged,
            this,
            [this]() {
                m_impl->peakLevel.setLevelRight(
                        m_impl->peakLevelMeterRight.level());
            });

    QObject::connect(
            &m_impl->rmsLevelMeterLeft,
            &RmsLevelMeter::levelChanged,
            this,
            [this]() {
                m_impl->rmsLevel.setLevelLeft(
                        m_impl->rmsLevelMeterLeft.level());
            });
    QObject::connect(
            &m_impl->rmsLevelMeterRight,
            &RmsLevelMeter::levelChanged,
            this,
            [this]() {
                m_impl->rmsLevel.setLevelRight(
                        m_impl->rmsLevelMeterRight.level());
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
