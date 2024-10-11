// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/spectrum/gui/FxSpectrum.h>

#include <piejam/fx_modules/spectrum/spectrum_internal_id.h>
#include <piejam/fx_modules/spectrum/spectrum_module.h>

#include <piejam/gui/model/AudioStreamAmplifier.h>
#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/EnumParameter.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/SpectrumDataGenerator.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::spectrum::gui
{

using namespace piejam::gui::model;

struct FxSpectrum::Impl
{
    Impl(BusType busType)
        : busType{busType}
    {
    }

    BusType busType;

    AudioStreamAmplifier channelAAmplifier{1u};
    AudioStreamAmplifier channelBAmplifier{1u};

    SpectrumDataGenerator spectrumAGenerator{std::array{busType}};
    SpectrumDataGenerator spectrumBGenerator{std::array{busType}};

    std::unique_ptr<BoolParameter> activeA;
    std::unique_ptr<BoolParameter> activeB;
    std::unique_ptr<EnumParameter> channelA;
    std::unique_ptr<EnumParameter> channelB;
    std::unique_ptr<FloatParameter> gainA;
    std::unique_ptr<FloatParameter> gainB;
    std::unique_ptr<BoolParameter> freeze;
    std::unique_ptr<FxStream> stream;

    piejam::gui::model::SpectrumData spectrumDataA;
    piejam::gui::model::SpectrumData spectrumDataB;
};

FxSpectrum::FxSpectrum(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : FxModule{store_dispatch, state_change_subscriber, fx_mod_id}
    , m_impl{make_pimpl<Impl>(busType())}
{
    auto const& parameters = this->parameters();

    makeParameter(
            m_impl->activeA,
            parameters.at(to_underlying(parameter_key::stream_a_active)));

    if (m_impl->busType == BusType::Mono)
    {
        // There is no control visible in mono mode, so we subscribe when parent
        // is subscribed.
        connectSubscribableChild(*m_impl->activeA);
    }

    makeParameter(
            m_impl->activeB,
            parameters.at(to_underlying(parameter_key::stream_b_active)));

    makeParameter(
            m_impl->channelA,
            parameters.at(to_underlying(parameter_key::channel_a)));

    makeParameter(
            m_impl->channelB,
            parameters.at(to_underlying(parameter_key::channel_b)));

    makeParameter(
            m_impl->gainA,
            parameters.at(to_underlying(parameter_key::gain_a)));

    makeParameter(
            m_impl->gainB,
            parameters.at(to_underlying(parameter_key::gain_b)));

    makeParameter(
            m_impl->freeze,
            parameters.at(to_underlying(parameter_key::freeze)));

    makeStream(to_underlying(stream_key::input), m_impl->stream, streams());

    m_impl->spectrumAGenerator.setActive(0, false);
    m_impl->spectrumAGenerator.setChannel(0, StereoChannel::Left);

    if (m_impl->busType == BusType::Stereo)
    {
        m_impl->spectrumBGenerator.setActive(0, false);
        m_impl->spectrumBGenerator.setChannel(0, StereoChannel::Right);
    }

    QObject::connect(
            m_impl->stream.get(),
            &AudioStreamProvider::captured,
            &m_impl->channelAAmplifier,
            &AudioStreamAmplifier::update);

    QObject::connect(
            &m_impl->channelAAmplifier,
            &AudioStreamAmplifier::amplified,
            &m_impl->spectrumAGenerator,
            &SpectrumDataGenerator::update);

    QObject::connect(
            &m_impl->spectrumAGenerator,
            &SpectrumDataGenerator::generated,
            this,
            [this](std::span<SpectrumDataPoints const> dataPoints) {
                dataA()->set(dataPoints[0]);
            });

    if (m_impl->busType == BusType::Stereo)
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                &m_impl->channelBAmplifier,
                &AudioStreamAmplifier::update);

        QObject::connect(
                &m_impl->channelBAmplifier,
                &AudioStreamAmplifier::amplified,
                &m_impl->spectrumBGenerator,
                &SpectrumDataGenerator::update);

        QObject::connect(
                &m_impl->spectrumBGenerator,
                &SpectrumDataGenerator::generated,
                this,
                [this](std::span<SpectrumDataPoints const> dataPoints) {
                    dataB()->set(dataPoints[0]);
                });
    }

    QObject::connect(
            m_impl->activeA.get(),
            &BoolParameter::valueChanged,
            this,
            &FxSpectrum::onActiveAChanged);

    QObject::connect(
            m_impl->channelA.get(),
            &EnumParameter::valueChanged,
            this,
            &FxSpectrum::onChannelAChanged);

    QObject::connect(
            m_impl->gainA.get(),
            &FloatParameter::valueChanged,
            this,
            &FxSpectrum::onGainAChanged);

    if (m_impl->busType == BusType::Stereo)
    {
        QObject::connect(
                m_impl->activeB.get(),
                &BoolParameter::valueChanged,
                this,
                &FxSpectrum::onActiveBChanged);

        QObject::connect(
                m_impl->channelB.get(),
                &EnumParameter::valueChanged,
                this,
                &FxSpectrum::onChannelBChanged);

        QObject::connect(
                m_impl->gainB.get(),
                &FloatParameter::valueChanged,
                this,
                &FxSpectrum::onGainBChanged);
    }

    QObject::connect(
            m_impl->freeze.get(),
            &BoolParameter::valueChanged,
            this,
            &FxSpectrum::onFreezeChanged);
}

auto
FxSpectrum::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
}

auto
FxSpectrum::activeA() const noexcept -> BoolParameter*
{
    return m_impl->activeA.get();
}

auto
FxSpectrum::activeB() const noexcept -> BoolParameter*
{
    return m_impl->activeB.get();
}

auto
FxSpectrum::channelA() const noexcept -> EnumParameter*
{
    return m_impl->channelA.get();
}

auto
FxSpectrum::channelB() const noexcept -> EnumParameter*
{
    return m_impl->channelB.get();
}

auto
FxSpectrum::gainA() const noexcept -> FloatParameter*
{
    return m_impl->gainA.get();
}

auto
FxSpectrum::gainB() const noexcept -> FloatParameter*
{
    return m_impl->gainB.get();
}

auto
FxSpectrum::freeze() const noexcept -> BoolParameter*
{
    return m_impl->freeze.get();
}

auto
FxSpectrum::dataA() const noexcept -> SpectrumData*
{
    return &m_impl->spectrumDataA;
}

auto
FxSpectrum::dataB() const noexcept -> SpectrumData*
{
    return &m_impl->spectrumDataB;
}

void
FxSpectrum::clear()
{
    m_impl->spectrumDataA.clear();
    m_impl->spectrumDataB.clear();
}

void
FxSpectrum::onSubscribe()
{
    auto const sample_rate =
            observe_once(runtime::selectors::select_sample_rate)->current;
    m_impl->spectrumAGenerator.setSampleRate(sample_rate);
    m_impl->spectrumBGenerator.setSampleRate(sample_rate);
}

void
FxSpectrum::onActiveAChanged()
{
    bool const active = m_impl->activeA->value();
    m_impl->spectrumAGenerator.setActive(0, active);

    clear();
}

void
FxSpectrum::onActiveBChanged()
{
    bool const active = m_impl->activeB->value();
    m_impl->spectrumBGenerator.setActive(0, active);

    clear();
}

void
FxSpectrum::onChannelAChanged()
{
    auto const x = StereoChannel{m_impl->channelA->value()};
    m_impl->spectrumAGenerator.setChannel(0, x);

    clear();
}

void
FxSpectrum::onChannelBChanged()
{
    auto const x = StereoChannel{m_impl->channelB->value()};
    m_impl->spectrumBGenerator.setChannel(0, x);

    clear();
}

void
FxSpectrum::onGainAChanged()
{
    m_impl->channelAAmplifier.setGain(0, m_impl->gainA->value());
}

void
FxSpectrum::onGainBChanged()
{
    m_impl->channelBAmplifier.setGain(0, m_impl->gainB->value());
}

void
FxSpectrum::onFreezeChanged()
{
    bool const freeze = m_impl->freeze->value();
    m_impl->spectrumAGenerator.setFreeze(freeze);
    m_impl->spectrumBGenerator.setFreeze(freeze);
}

} // namespace piejam::fx_modules::spectrum::gui
