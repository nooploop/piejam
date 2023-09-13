// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelPerform.h>

#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/DbScaleData.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/gui/model/MixerDbScales.h>
#include <piejam/gui/model/StereoLevelParameter.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct MixerChannelPerform::Impl
{
    runtime::mixer::channel_id busId;

    std::unique_ptr<StereoLevelParameter> peakLevel;
    std::unique_ptr<StereoLevelParameter> rmsLevel;
    std::unique_ptr<FloatParameter> volume;
    std::unique_ptr<FloatParameter> panBalance;
    std::unique_ptr<BoolParameter> record;
    std::unique_ptr<BoolParameter> solo;
    std::unique_ptr<BoolParameter> mute;
};

MixerChannelPerform::MixerChannelPerform(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(id))
    , m_busType(toBusType(observe_once(
              runtime::selectors::make_mixer_channel_bus_type_selector(id))))
{
    makeParameter(
            m_impl->peakLevel,
            observe_once(
                    runtime::selectors::
                            make_mixer_channel_peak_level_parameter_selector(
                                    id)));

    makeParameter(
            m_impl->rmsLevel,
            observe_once(
                    runtime::selectors::
                            make_mixer_channel_rms_level_parameter_selector(
                                    id)));

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

MixerChannelPerform::~MixerChannelPerform() = default;

auto
MixerChannelPerform::peakLevel() const noexcept -> StereoLevelParameter*
{
    return m_impl->peakLevel.get();
}

auto
MixerChannelPerform::rmsLevel() const noexcept -> StereoLevelParameter*
{
    return m_impl->rmsLevel.get();
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
    observe(runtime::selectors::make_mixer_channel_name_selector(m_impl->busId),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_muted_by_solo_selector(m_impl->busId),
            [this](bool x) { setMutedBySolo(x); });
}

} // namespace piejam::gui::model
