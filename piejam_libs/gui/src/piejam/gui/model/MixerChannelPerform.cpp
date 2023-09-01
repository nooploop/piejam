// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelPerform.h>

#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/DbScaleData.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/gui/model/MixerDbScales.h>
#include <piejam/math.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

namespace
{

[[nodiscard]] constexpr auto
toLevel(float const l)
{
    constexpr auto min = math::dB_to_linear(-60.01f);
    return math::linear_to_dB(l, min);
}

[[nodiscard]] constexpr auto
toVolume(float const v)
{
    constexpr auto min = math::dB_to_linear(-60.01f);
    return math::linear_to_dB(v, min);
}

} // namespace

struct MixerChannelPerform::Impl
{
    runtime::mixer::channel_id busId;
    audio::bus_type bus_type;
    runtime::float_parameter_id volume;
    runtime::stereo_level_parameter_id level;

    std::unique_ptr<MidiAssignable> volumeMidi;

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
    , m_impl(std::make_unique<Impl>(
              id,
              observe_once(
                      runtime::selectors::make_mixer_channel_bus_type_selector(
                              id)),
              observe_once(
                      runtime::selectors::
                              make_mixer_channel_volume_parameter_selector(id)),
              observe_once(
                      runtime::selectors::
                              make_mixer_channel_level_parameter_selector(id)),
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::
                                      make_mixer_channel_volume_parameter_selector(
                                              id))),
              std::make_unique<FloatParameter>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::
                                      make_mixer_channel_pan_balance_parameter_selector(
                                              id))),
              std::make_unique<BoolParameter>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::
                                      make_mixer_channel_record_parameter_selector(
                                              id))),
              std::make_unique<BoolParameter>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::
                                      make_mixer_channel_solo_parameter_selector(
                                              id))),
              std::make_unique<BoolParameter>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::
                                      make_mixer_channel_mute_parameter_selector(
                                              id)))))
    , m_busType(toBusType(m_impl->bus_type))
{
}

MixerChannelPerform::~MixerChannelPerform() = default;

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

    observe(runtime::selectors::make_float_parameter_value_selector(
                    m_impl->volume),
            [this](float x) {
                setVolume(g_mixerDbScales.volumeFaderScale()->dBToPosition(
                        toVolume(x)));
            });

    observe(runtime::selectors::make_muted_by_solo_selector(m_impl->busId),
            [this](bool x) { setMutedBySolo(x); });

    observe(runtime::selectors::make_level_parameter_value_selector(
                    m_impl->level),
            [this](runtime::stereo_level const& x) {
                setLevel(
                        g_mixerDbScales.levelMeterScale()->dBToPosition(
                                toLevel(x.left)),
                        g_mixerDbScales.levelMeterScale()->dBToPosition(
                                toLevel(x.right)));
            });
}

void
MixerChannelPerform::changeVolume(double position)
{
    dispatch(runtime::actions::set_float_parameter(
            m_impl->volume,
            static_cast<float>(math::dB_to_linear(
                    g_mixerDbScales.volumeFaderScale()->dBAt(position)))));
}

auto
MixerChannelPerform::volumeMidi() const noexcept -> MidiAssignable*
{
    return m_impl->volumeMidi.get();
}

} // namespace piejam::gui::model
