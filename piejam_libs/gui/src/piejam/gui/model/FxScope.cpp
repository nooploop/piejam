// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxScope.h>

#include <piejam/audio/types.h>
#include <piejam/functional/in_interval.h>
#include <piejam/gui/model/AudioStreamChannelDuplicator.h>
#include <piejam/gui/model/FxEnumParameter.h>
#include <piejam/gui/model/FxFloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/ScopeDataGenerator.h>
#include <piejam/gui/model/TriggerSlope.h>
#include <piejam/gui/model/WaveformDataGenerator.h>
#include <piejam/runtime/modules/scope/scope_module.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::gui::model
{

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

    WaveformDataGenerator waveformGenerator;
    AudioStreamChannelDuplicator channelDuplicator;
    ScopeDataGenerator scopeDataGenerator;

    std::unique_ptr<FxEnumParameter> triggerSource;
    std::unique_ptr<FxEnumParameter> triggerSlope;
    std::unique_ptr<FxFloatParameter> triggerLevel;
    std::unique_ptr<AudioStreamProvider> stream;
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

    auto const triggerSourceKey =
            to_underlying(runtime::modules::scope::parameter_key::mode);
    m_impl->triggerSource = std::make_unique<FxEnumParameter>(
            dispatch(),
            this->state_change_subscriber(),
            FxParameterKeyId{
                    .key = triggerSourceKey,
                    .id = parameters->at(triggerSourceKey)});
    connectSubscribableChild(*m_impl->triggerSource);

    auto const triggerSlopeKey = to_underlying(
            runtime::modules::scope::parameter_key::trigger_slope);
    m_impl->triggerSlope = std::make_unique<FxEnumParameter>(
            dispatch(),
            this->state_change_subscriber(),
            FxParameterKeyId{
                    .key = triggerSlopeKey,
                    .id = parameters->at(triggerSlopeKey)});
    connectSubscribableChild(*m_impl->triggerSlope);

    auto const triggerLevelKey = to_underlying(
            runtime::modules::scope::parameter_key::trigger_level);
    m_impl->triggerLevel = std::make_unique<FxFloatParameter>(
            dispatch(),
            this->state_change_subscriber(),
            FxParameterKeyId{
                    .key = triggerLevelKey,
                    .id = parameters->at(triggerLevelKey)});
    connectSubscribableChild(*m_impl->triggerLevel);

    auto const streams = observe_once(
            runtime::selectors::make_fx_module_streams_selector(fx_mod_id));

    constexpr auto streamKey =
            to_underlying(runtime::modules::scope::stream_key::input);
    m_impl->stream = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            FxStreamKeyId{.key = streamKey, .id = streams->at(streamKey)});
    connectSubscribableChild(*m_impl->stream);

    m_impl->waveformGenerator.setActive(0, activeA());
    m_impl->waveformGenerator.setChannel(0, channelA());
    m_impl->scopeDataGenerator.setActive(0, activeA());
    m_impl->scopeDataGenerator.setChannel(0, channelA());
    m_impl->scopeDataGenerator.setHoldTime(
            std::chrono::milliseconds{m_holdTime});

    if (m_impl->busType == BusType::Stereo)
    {
        m_impl->waveformGenerator.setActive(1, activeB());
        m_impl->waveformGenerator.setChannel(1, channelB());
        m_impl->scopeDataGenerator.setActive(1, activeB());
        m_impl->scopeDataGenerator.setChannel(1, channelB());
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
                &m_impl->waveformGenerator,
                &WaveformDataGenerator::update);

        QObject::connect(
                &m_impl->channelDuplicator,
                &AudioStreamChannelDuplicator::duplicated,
                &m_impl->scopeDataGenerator,
                &ScopeDataGenerator::update);
    }
    else
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                &m_impl->waveformGenerator,
                &WaveformDataGenerator::update);

        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                &m_impl->scopeDataGenerator,
                &ScopeDataGenerator::update);
    }

    QObject::connect(
            m_impl->triggerSource.get(),
            &FxEnumParameter::valueChanged,
            this,
            &FxScope::onTriggerSourceChanged);

    QObject::connect(
            m_impl->triggerSlope.get(),
            &FxEnumParameter::valueChanged,
            this,
            &FxScope::onTriggerSlopeChanged);

    QObject::connect(
            m_impl->triggerLevel.get(),
            &FxFloatParameter::valueChanged,
            this,
            &FxScope::onTriggerLevelChanged);
}

FxScope::~FxScope() = default;

auto
FxScope::busType() const noexcept -> BusType
{
    return m_impl->busType;
}

auto
FxScope::triggerSource() const noexcept -> FxEnumParameter*
{
    return m_impl->triggerSource.get();
}

auto
FxScope::triggerSlope() const noexcept -> FxEnumParameter*
{
    return m_impl->triggerSlope.get();
}

auto
FxScope::triggerLevel() const noexcept -> FxFloatParameter*
{
    return m_impl->triggerLevel.get();
}

void
FxScope::setSamplesPerPixel(int const x)
{
    if (m_samplesPerPixel != x)
    {
        m_samplesPerPixel = x;
        m_impl->waveformGenerator.setSamplesPerPixel(x);
        emit samplesPerPixelChanged();
    }
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
FxScope::setHoldTime(int holdTime)
{
    if (holdTime < 16 || holdTime > 1600)
    {
        return;
    }

    if (m_holdTime != holdTime)
    {
        m_holdTime = holdTime;
        m_impl->scopeDataGenerator.setHoldTime(
                std::chrono::milliseconds{m_holdTime});
        emit holdTimeChanged();
    }
}

void
FxScope::changeActiveA(bool const active)
{
    if (m_activeA != active)
    {
        m_activeA = active;
        m_impl->waveformGenerator.setActive(0, active);
        m_impl->scopeDataGenerator.setActive(0, active);
        emit activeAChanged();

        clear();
    }
}

void
FxScope::changeChannelA(piejam::gui::model::StereoChannel const x)
{
    if (m_channelA != x)
    {
        m_channelA = x;
        m_impl->waveformGenerator.setChannel(0, x);
        m_impl->scopeDataGenerator.setChannel(0, x);
        emit channelAChanged();

        clear();
    }
}

void
FxScope::changeActiveB(bool const active)
{
    if (m_activeB != active)
    {
        m_activeB = active;
        m_impl->waveformGenerator.setActive(1, active);
        m_impl->scopeDataGenerator.setActive(1, active);
        emit activeBChanged();

        clear();
    }
}

void
FxScope::changeChannelB(piejam::gui::model::StereoChannel const x)
{
    if (m_channelB != x)
    {
        m_channelB = x;
        m_impl->waveformGenerator.setChannel(1, x);
        m_impl->scopeDataGenerator.setChannel(1, x);
        emit channelBChanged();

        clear();
    }
}

void
FxScope::setScopeResolution(int x)
{
    if (in_closed(x, 1, 8))
    {
        if (m_scopeResolution != x)
        {
            m_scopeResolution = x;
            m_impl->scopeDataGenerator.setResolution(
                    static_cast<std::size_t>(x));
            emit scopeResolutionChanged();
        }
    }
}

void
FxScope::setFreeze(bool x)
{
    if (m_freeze != x)
    {
        m_freeze = x;
        m_impl->scopeDataGenerator.setFreeze(x);
        emit freezeChanged();
    }
}

void
FxScope::clear()
{
    m_waveformDataA.get().clear();
    m_waveformDataB.get().clear();
    m_scopeDataA.clear();
    m_scopeDataB.clear();

    if (m_activeA)
    {
        m_waveformDataA.get().resize(m_viewSize);
    }

    if (m_activeB)
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
            observe_once(runtime::selectors::select_sample_rate).second);
}

void
FxScope::onTriggerSourceChanged()
{
    switch (static_cast<runtime::fx::parameter_key>(
            m_impl->triggerSource->value()))
    {
        case to_underlying(runtime::modules::scope::mode::trigger_a):
            m_impl->scopeDataGenerator.setTriggerStream(0);
            break;

        case to_underlying(runtime::modules::scope::mode::trigger_b):
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

} // namespace piejam::gui::model
