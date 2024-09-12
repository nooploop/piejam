// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/scope/gui/FxScope.h>

#include <piejam/audio/types.h>
#include <piejam/functional/in_interval.h>
#include <piejam/fx_modules/scope/scope_internal_id.h>
#include <piejam/fx_modules/scope/scope_module.h>
#include <piejam/gui/model/AudioStreamAmplifier.h>
#include <piejam/gui/model/AudioStreamChannelDuplicator.h>
#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/EnumParameter.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/ScopeDataGenerator.h>
#include <piejam/gui/model/WaveformDataGenerator.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::scope::gui
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

struct FxScope::Impl
{
    Impl(BusType busType)
        : busType{busType}
        , waveformGenerator{substreamConfigs(busType)}
        , scopeDataGenerator{substreamConfigs(busType)}
    {
    }

    BusType busType;

    AudioStreamAmplifier channelAmplifier{busType == BusType::Stereo ? 2u : 1u};
    AudioStreamChannelDuplicator channelDuplicator;
    WaveformDataGenerator waveformGenerator;
    ScopeDataGenerator scopeDataGenerator;

    std::unique_ptr<EnumParameter> mode;
    std::unique_ptr<EnumParameter> triggerSlope;
    std::unique_ptr<FloatParameter> triggerLevel;
    std::unique_ptr<FloatParameter> holdTime;
    std::unique_ptr<IntParameter> waveformWindowSize;
    std::unique_ptr<IntParameter> scopeWindowSize;
    std::unique_ptr<BoolParameter> activeA;
    std::unique_ptr<BoolParameter> activeB;
    std::unique_ptr<EnumParameter> channelA;
    std::unique_ptr<EnumParameter> channelB;
    std::unique_ptr<FloatParameter> gainA;
    std::unique_ptr<FloatParameter> gainB;
    std::unique_ptr<BoolParameter> freeze;
    std::unique_ptr<FxStream> stream;
};

FxScope::FxScope(
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
            m_impl->mode,
            parameters->at(to_underlying(parameter_key::mode)));

    makeParameter(
            m_impl->triggerSlope,
            parameters->at(to_underlying(parameter_key::trigger_slope)));

    makeParameter(
            m_impl->triggerLevel,
            parameters->at(to_underlying(parameter_key::trigger_level)));

    makeParameter(
            m_impl->holdTime,
            parameters->at(to_underlying(parameter_key::hold_time)));

    makeParameter(
            m_impl->waveformWindowSize,
            parameters->at(to_underlying(parameter_key::waveform_window_size)));

    makeParameter(
            m_impl->scopeWindowSize,
            parameters->at(to_underlying(parameter_key::scope_window_size)));

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

    m_impl->waveformGenerator.setActive(0, true);
    m_impl->waveformGenerator.setChannel(0, StereoChannel::Left);
    m_impl->scopeDataGenerator.setActive(0, true);
    m_impl->scopeDataGenerator.setChannel(0, StereoChannel::Left);
    m_impl->scopeDataGenerator.setHoldTime(std::chrono::milliseconds{80});

    if (m_impl->busType == BusType::Stereo)
    {
        m_impl->waveformGenerator.setActive(1, false);
        m_impl->waveformGenerator.setChannel(1, StereoChannel::Right);
        m_impl->scopeDataGenerator.setActive(1, false);
        m_impl->scopeDataGenerator.setChannel(1, StereoChannel::Right);
    }

    QObject::connect(
            &m_impl->waveformGenerator,
            &WaveformDataGenerator::generated,
            this,
            [this](std::span<WaveformData const> addedLines) {
                waveformDataA()->get().shift_push_back(addedLines[0]);
                waveformDataA()->update();

                if (m_impl->busType == BusType::Stereo)
                {
                    waveformDataB()->get().shift_push_back(addedLines[1]);
                    waveformDataB()->update();
                }
            });

    QObject::connect(
            &m_impl->scopeDataGenerator,
            &ScopeDataGenerator::generated,
            this,
            [this](std::span<ScopeData::Samples const> scopeDataSamples) {
                m_scopeDataA.set(scopeDataSamples[0]);

                if (m_impl->busType == BusType::Stereo)
                {
                    m_scopeDataB.set(scopeDataSamples[1]);
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
                &m_impl->waveformGenerator,
                &WaveformDataGenerator::update);

        QObject::connect(
                &m_impl->channelAmplifier,
                &AudioStreamAmplifier::amplified,
                &m_impl->scopeDataGenerator,
                &ScopeDataGenerator::update);
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
                &m_impl->waveformGenerator,
                &WaveformDataGenerator::update);

        QObject::connect(
                &m_impl->channelAmplifier,
                &AudioStreamAmplifier::amplified,
                &m_impl->scopeDataGenerator,
                &ScopeDataGenerator::update);
    }

    QObject::connect(
            m_impl->mode.get(),
            &EnumParameter::valueChanged,
            this,
            &FxScope::onTriggerSourceChanged);

    QObject::connect(
            m_impl->triggerSlope.get(),
            &EnumParameter::valueChanged,
            this,
            &FxScope::onTriggerSlopeChanged);

    QObject::connect(
            m_impl->triggerLevel.get(),
            &FloatParameter::valueChanged,
            this,
            &FxScope::onTriggerLevelChanged);

    QObject::connect(
            m_impl->holdTime.get(),
            &FloatParameter::valueChanged,
            this,
            &FxScope::onHoldTimeChanged);

    QObject::connect(
            m_impl->waveformWindowSize.get(),
            &IntParameter::valueChanged,
            this,
            &FxScope::onWaveformWindowSizeChanged);

    QObject::connect(
            m_impl->scopeWindowSize.get(),
            &IntParameter::valueChanged,
            this,
            &FxScope::onScopeWindowSizeChanged);

    QObject::connect(
            m_impl->activeA.get(),
            &BoolParameter::valueChanged,
            this,
            &FxScope::onActiveAChanged);

    QObject::connect(
            m_impl->channelA.get(),
            &EnumParameter::valueChanged,
            this,
            &FxScope::onChannelAChanged);

    QObject::connect(
            m_impl->gainA.get(),
            &FloatParameter::valueChanged,
            this,
            &FxScope::onGainAChanged);

    if (m_impl->busType == BusType::Stereo)
    {
        QObject::connect(
                m_impl->activeB.get(),
                &BoolParameter::valueChanged,
                this,
                &FxScope::onActiveBChanged);

        QObject::connect(
                m_impl->channelB.get(),
                &EnumParameter::valueChanged,
                this,
                &FxScope::onChannelBChanged);

        QObject::connect(
                m_impl->gainB.get(),
                &FloatParameter::valueChanged,
                this,
                &FxScope::onGainBChanged);
    }

    QObject::connect(
            m_impl->freeze.get(),
            &BoolParameter::valueChanged,
            this,
            &FxScope::onFreezeChanged);
}

FxScope::~FxScope() = default;

auto
FxScope::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
}

auto
FxScope::busType() const noexcept -> BusType
{
    return m_impl->busType;
}

auto
FxScope::mode() const noexcept -> EnumParameter*
{
    return m_impl->mode.get();
}

auto
FxScope::triggerSlope() const noexcept -> EnumParameter*
{
    return m_impl->triggerSlope.get();
}

auto
FxScope::triggerLevel() const noexcept -> FloatParameter*
{
    return m_impl->triggerLevel.get();
}

auto
FxScope::holdTime() const noexcept -> FloatParameter*
{
    return m_impl->holdTime.get();
}

auto
FxScope::waveformWindowSize() const noexcept -> IntParameter*
{
    return m_impl->waveformWindowSize.get();
}

auto
FxScope::scopeWindowSize() const noexcept -> IntParameter*
{
    return m_impl->scopeWindowSize.get();
}

auto
FxScope::activeA() const noexcept -> BoolParameter*
{
    return m_impl->activeA.get();
}

auto
FxScope::activeB() const noexcept -> BoolParameter*
{
    return m_impl->activeB.get();
}

auto
FxScope::channelA() const noexcept -> EnumParameter*
{
    return m_impl->channelA.get();
}

auto
FxScope::channelB() const noexcept -> EnumParameter*
{
    return m_impl->channelB.get();
}

auto
FxScope::gainA() const noexcept -> FloatParameter*
{
    return m_impl->gainA.get();
}

auto
FxScope::gainB() const noexcept -> FloatParameter*
{
    return m_impl->gainB.get();
}

auto
FxScope::freeze() const noexcept -> BoolParameter*
{
    return m_impl->freeze.get();
}

void
FxScope::setViewSize(int const x)
{
    if (m_viewSize != x)
    {
        m_viewSize = x;
        m_impl->scopeDataGenerator.setWindowSize(static_cast<std::size_t>(x));
        emit viewSizeChanged();

        clear();
    }
}

void
FxScope::clear()
{
    m_waveformDataA.get().clear();
    m_waveformDataB.get().clear();
    m_scopeDataA.clear();
    m_scopeDataB.clear();

    if (m_impl->activeA->value())
    {
        m_waveformDataA.get().resize(m_viewSize);
    }

    if (m_impl->activeB->value())
    {
        m_waveformDataB.get().resize(m_viewSize);
    }

    m_impl->waveformGenerator.clear();
    m_impl->scopeDataGenerator.clear();

    m_waveformDataA.update();
    m_waveformDataB.update();
}

void
FxScope::onSubscribe()
{
    m_impl->scopeDataGenerator.setSampleRate(
            observe_once(runtime::selectors::select_sample_rate)->current);
}

void
FxScope::onTriggerSourceChanged()
{
    switch (static_cast<runtime::fx::parameter_key>(m_impl->mode->value()))
    {
        case to_underlying(mode::trigger_a):
            m_impl->scopeDataGenerator.setTriggerStream(0);
            break;

        case to_underlying(mode::trigger_b):
            m_impl->scopeDataGenerator.setTriggerStream(1);
            break;

        default:
            break;
    }

    clear();
}

void
FxScope::onTriggerSlopeChanged()
{
    m_impl->scopeDataGenerator.setTriggerSlope(
            static_cast<TriggerSlope>(m_impl->triggerSlope->value()));
}

void
FxScope::onTriggerLevelChanged()
{
    m_impl->scopeDataGenerator.setTriggerLevel(
            static_cast<float>(m_impl->triggerLevel->value()));
}

void
FxScope::onHoldTimeChanged()
{
    m_impl->scopeDataGenerator.setHoldTime(std::chrono::milliseconds{
            static_cast<int>(m_impl->holdTime->value())});
}

void
FxScope::onWaveformWindowSizeChanged()
{
    m_impl->waveformGenerator.setSamplesPerPixel(
            std::pow(2, m_impl->waveformWindowSize->value() * 3));
}

void
FxScope::onScopeWindowSizeChanged()
{
    m_impl->scopeDataGenerator.setResolution(
            m_impl->scopeWindowSize->value() * 2 + 1);
}

void
FxScope::onActiveAChanged()
{
    bool const active = m_impl->activeA->value();
    m_impl->waveformGenerator.setActive(0, active);
    m_impl->scopeDataGenerator.setActive(0, active);

    clear();
}

void
FxScope::onActiveBChanged()
{
    bool const active = m_impl->activeB->value();
    m_impl->waveformGenerator.setActive(1, active);
    m_impl->scopeDataGenerator.setActive(1, active);

    clear();
}

void
FxScope::onChannelAChanged()
{
    auto const x = StereoChannel{m_impl->channelA->value()};
    m_impl->waveformGenerator.setChannel(0, x);
    m_impl->scopeDataGenerator.setChannel(0, x);

    clear();
}

void
FxScope::onChannelBChanged()
{
    auto const x = StereoChannel{m_impl->channelB->value()};
    m_impl->waveformGenerator.setChannel(1, x);
    m_impl->scopeDataGenerator.setChannel(1, x);

    clear();
}

void
FxScope::onGainAChanged()
{
    m_impl->channelAmplifier.setGain(0, m_impl->gainA->value());
}

void
FxScope::onGainBChanged()
{
    m_impl->channelAmplifier.setGain(1, m_impl->gainB->value());
}

void
FxScope::onFreezeChanged()
{
    bool const freeze = m_impl->freeze->value();
    m_impl->scopeDataGenerator.setFreeze(freeze);
    m_impl->waveformGenerator.setFreeze(freeze);
}

} // namespace piejam::fx_modules::scope::gui
