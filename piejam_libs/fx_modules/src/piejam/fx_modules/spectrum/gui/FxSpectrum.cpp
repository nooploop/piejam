// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/spectrum/gui/FxSpectrum.h>

#include <piejam/fx_modules/spectrum/spectrum_internal_id.h>
#include <piejam/fx_modules/spectrum/spectrum_module.h>
#include <piejam/gui/model/AudioStreamAmplifier.h>
#include <piejam/gui/model/AudioStreamChannelDuplicator.h>
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

namespace
{

auto
substreamConfigs(BusType busType) -> std::span<BusType const>
{
    switch (busType)
    {
        case BusType::Mono:
        {
            static std::array const configs{BusType::Mono};
            return configs;
        }

        case BusType::Stereo:
        {
            static std::array const configs{BusType::Stereo, BusType::Stereo};
            return configs;
        }
    }
}

} // namespace

struct FxSpectrum::Impl
{
    Impl(BusType busType)
        : busType{busType}
    {
    }

    BusType busType;

    AudioStreamAmplifier channelAmplifier{busType == BusType::Stereo ? 2u : 1u};
    AudioStreamChannelDuplicator channelDuplicator;
    SpectrumDataGenerator spectrumGenerator{substreamConfigs(busType)};

    std::unique_ptr<BoolParameter> activeA;
    std::unique_ptr<BoolParameter> activeB;
    std::unique_ptr<EnumParameter> channelA;
    std::unique_ptr<EnumParameter> channelB;
    std::unique_ptr<FloatParameter> gainA;
    std::unique_ptr<FloatParameter> gainB;
    std::unique_ptr<BoolParameter> freeze;
    std::unique_ptr<FxStream> stream;
};

FxSpectrum::FxSpectrum(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(toBusType(
              observe_once(runtime::selectors::make_fx_module_bus_type_selector(
                      fx_mod_id)))))
{
    auto const parameters = observe_once(
            runtime::selectors::make_fx_module_parameters_selector(fx_mod_id));

    makeParameter(
            m_impl->activeA,
            parameters->at(to_underlying(parameter_key::stream_a_active)));

    makeParameter(
            m_impl->activeB,
            parameters->at(to_underlying(parameter_key::stream_b_active)));

    makeParameter(
            m_impl->channelA,
            parameters->at(to_underlying(parameter_key::channel_a)));

    makeParameter(
            m_impl->channelB,
            parameters->at(to_underlying(parameter_key::channel_b)));

    makeParameter(
            m_impl->gainA,
            parameters->at(to_underlying(parameter_key::gain_a)));

    makeParameter(
            m_impl->gainB,
            parameters->at(to_underlying(parameter_key::gain_b)));

    makeParameter(
            m_impl->freeze,
            parameters->at(to_underlying(parameter_key::freeze)));

    auto const streams = observe_once(
            runtime::selectors::make_fx_module_streams_selector(fx_mod_id));

    makeStream(to_underlying(stream_key::input), m_impl->stream, *streams);

    m_impl->spectrumGenerator.setActive(0, false);
    m_impl->spectrumGenerator.setChannel(0, StereoChannel::Left);

    if (m_impl->busType == BusType::Stereo)
    {
        m_impl->spectrumGenerator.setActive(1, false);
        m_impl->spectrumGenerator.setChannel(1, StereoChannel::Right);
    }

    QObject::connect(
            &m_impl->spectrumGenerator,
            &SpectrumDataGenerator::generated,
            this,
            [this](std::span<SpectrumDataPoints const> dataPoints) {
                dataA()->set(dataPoints[0]);

                if (m_impl->busType == BusType::Stereo)
                {
                    dataB()->set(dataPoints[1]);
                }
            });

    if (m_impl->busType == BusType::Stereo)
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                &m_impl->channelDuplicator,
                &AudioStreamChannelDuplicator::update);

        QObject::connect(
                &m_impl->channelDuplicator,
                &AudioStreamChannelDuplicator::duplicated,
                &m_impl->channelAmplifier,
                &AudioStreamAmplifier::update);

        QObject::connect(
                &m_impl->channelAmplifier,
                &AudioStreamAmplifier::amplified,
                &m_impl->spectrumGenerator,
                &SpectrumDataGenerator::update);
    }
    else
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                &m_impl->channelAmplifier,
                &AudioStreamAmplifier::update);

        QObject::connect(
                &m_impl->channelAmplifier,
                &AudioStreamAmplifier::amplified,
                &m_impl->spectrumGenerator,
                &SpectrumDataGenerator::update);
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

FxSpectrum::~FxSpectrum() = default;

auto
FxSpectrum::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
}

auto
FxSpectrum::busType() const noexcept -> BusType
{
    return m_impl->busType;
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

void
FxSpectrum::onSubscribe()
{
    m_impl->spectrumGenerator.setSampleRate(
            observe_once(runtime::selectors::select_sample_rate)->current);
}

void
FxSpectrum::onActiveAChanged()
{
    bool const active = m_impl->activeA->value();
    m_impl->spectrumGenerator.setActive(0, active);

    clear();
}

void
FxSpectrum::onActiveBChanged()
{
    bool const active = m_impl->activeB->value();
    m_impl->spectrumGenerator.setActive(1, active);

    clear();
}

void
FxSpectrum::onChannelAChanged()
{
    auto const x = StereoChannel{m_impl->channelA->value()};
    m_impl->spectrumGenerator.setChannel(0, x);

    clear();
}

void
FxSpectrum::onChannelBChanged()
{
    auto const x = StereoChannel{m_impl->channelB->value()};
    m_impl->spectrumGenerator.setChannel(1, x);

    clear();
}

void
FxSpectrum::onGainAChanged()
{
    m_impl->channelAmplifier.setGain(0, m_impl->gainA->value());
}

void
FxSpectrum::onGainBChanged()
{
    m_impl->channelAmplifier.setGain(1, m_impl->gainB->value());
}

void
FxSpectrum::onFreezeChanged()
{
    bool const freeze = m_impl->freeze->value();
    m_impl->spectrumGenerator.setFreeze(freeze);
}

} // namespace piejam::fx_modules::spectrum::gui
