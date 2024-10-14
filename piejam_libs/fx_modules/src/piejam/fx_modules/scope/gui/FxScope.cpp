// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/scope/gui/FxScope.h>

#include <piejam/fx_modules/scope/scope_internal_id.h>
#include <piejam/fx_modules/scope/scope_module.h>

#include <piejam/audio/types.h>
#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/EnumParameter.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/ScopeDataGenerator.h>
#include <piejam/gui/model/StreamProcessor.h>
#include <piejam/gui/model/StreamSamplesCache.h>
#include <piejam/gui/model/WaveformGenerator.h>
#include <piejam/renew.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::scope::gui
{

using namespace piejam::gui::model;

namespace
{

struct ScopeStreamProcessor : StreamProcessor<ScopeStreamProcessor>
{
    template <class Samples>
    void process(Samples&& samples)
    {
        if (processAsWaveform)
        {
            waveform.update(
                    waveformGenerator.process(std::forward<Samples>(samples)));
        }
        else
        {
            scopeDataCache.process(std::forward<Samples>(samples));
        }
    }

    bool processAsWaveform{true};

    WaveformGenerator waveformGenerator;
    StreamSamplesCache scopeDataCache;

    WaveformSlot waveform;
    ScopeData scopeData;
};

} // namespace

struct FxScope::Impl
{
    static constexpr audio::sample_rate default_sample_rate{48000u};

    Impl(BusType busType)
        : busType{busType}
    {
    }

    BusType busType;
    audio::sample_rate sample_rate{default_sample_rate};

    std::unique_ptr<EnumParameter> mode;
    std::unique_ptr<EnumParameter> triggerSlope;
    std::unique_ptr<FloatParameter> triggerLevel;
    std::unique_ptr<FloatParameter> holdTime;
    std::unique_ptr<IntParameter> waveformResolution;
    std::unique_ptr<IntParameter> scopeResolution;
    std::unique_ptr<BoolParameter> freeze;
    std::unique_ptr<FxStream> stream;

    std::pair<ScopeStreamProcessor, ScopeStreamProcessor> streamProcessor;
    ScopeDataGenerator scopeDataGenerator;

    auto modeEnum() const noexcept -> Mode
    {
        return mode->as<Mode>();
    }

    auto triggerSlopeEnum() const noexcept -> TriggerSlope
    {
        return triggerSlope->as<TriggerSlope>();
    }

    auto holdTimeInFrames() const noexcept -> std::size_t
    {
        return sample_rate.to_samples(
                std::chrono::milliseconds{static_cast<int>(holdTime->value())});
    }

    void updateSampleRate(audio::sample_rate sr)
    {
        if (sr.valid())
        {
            sample_rate = sr;
        }
    }

    void updateMode()
    {
        bool processAsWaveform = modeEnum() == Mode::Free;
        streamProcessor.first.processAsWaveform = processAsWaveform;
        streamProcessor.second.processAsWaveform = processAsWaveform;
    }

    void updateWaveformGenerator()
    {
        int samplesPerPixel = std::pow(2, waveformResolution->value() * 3);
        renew(streamProcessor.first.waveformGenerator, samplesPerPixel);
        renew(streamProcessor.second.waveformGenerator, samplesPerPixel);
    }

    void updateWaveform(std::size_t viewSize)
    {
        streamProcessor.first.waveform.resize(viewSize);
        streamProcessor.second.waveform.resize(viewSize);
    }

    void updateScopeSamplesCache(std::size_t viewSize)
    {

        std::size_t doubleViewSize = viewSize * 2;
        int stride = scopeResolution->value() * 2 + 1;
        renew(streamProcessor.first.scopeDataCache, doubleViewSize, stride);
        renew(streamProcessor.second.scopeDataCache, doubleViewSize, stride);
    }
};

FxScope::FxScope(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : FxModule{store_dispatch, state_change_subscriber, fx_mod_id}
    , m_impl{make_pimpl<Impl>(busType())}
{
    auto const& parameters = this->parameters();

    makeParameter(
            m_impl->mode,
            parameters.at(to_underlying(parameter_key::mode)));

    makeParameter(
            m_impl->triggerSlope,
            parameters.at(to_underlying(parameter_key::trigger_slope)));

    makeParameter(
            m_impl->triggerLevel,
            parameters.at(to_underlying(parameter_key::trigger_level)));

    makeParameter(
            m_impl->holdTime,
            parameters.at(to_underlying(parameter_key::hold_time)));

    makeParameter(
            m_impl->waveformResolution,
            parameters.at(to_underlying(parameter_key::waveform_window_size)));

    makeParameter(
            m_impl->scopeResolution,
            parameters.at(to_underlying(parameter_key::scope_window_size)));

    makeParameter(
            m_impl->streamProcessor.first.active,
            parameters.at(to_underlying(parameter_key::stream_a_active)));

    makeParameter(
            m_impl->streamProcessor.second.active,
            parameters.at(to_underlying(parameter_key::stream_b_active)));

    makeParameter(
            m_impl->streamProcessor.first.channel,
            parameters.at(to_underlying(parameter_key::channel_a)));

    makeParameter(
            m_impl->streamProcessor.second.channel,
            parameters.at(to_underlying(parameter_key::channel_b)));

    makeParameter(
            m_impl->streamProcessor.first.gain,
            parameters.at(to_underlying(parameter_key::gain_a)));

    makeParameter(
            m_impl->streamProcessor.second.gain,
            parameters.at(to_underlying(parameter_key::gain_b)));

    makeParameter(
            m_impl->freeze,
            parameters.at(to_underlying(parameter_key::freeze)));

    makeStream(to_underlying(stream_key::input), m_impl->stream, streams());

    auto clear_fn = [this]() { clear(); };

    if (m_impl->busType == BusType::Mono)
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                this,
                [this](AudioStream captured) {
                    if (m_impl->freeze->value())
                    {
                        return;
                    }

                    BOOST_ASSERT(captured.num_channels() == 1);
                    m_impl->streamProcessor.first.processSamples(
                            captured.samples());

                    if (m_impl->modeEnum() != Mode::Free)
                    {
                        auto scopeData = m_impl->scopeDataGenerator.process(
                                0,
                                {m_impl->streamProcessor.first.scopeDataCache
                                         .cached()},
                                m_viewSize,
                                m_impl->triggerSlopeEnum(),
                                m_impl->triggerLevel->valueF(),
                                captured.num_frames(),
                                m_impl->holdTimeInFrames());

                        if (scopeData.size() == 1)
                        {
                            m_impl->streamProcessor.first.scopeData.set(
                                    scopeData[0]);
                        }
                    }
                });
    }
    else
    {
        QObject::connect(
                m_impl->stream.get(),
                &AudioStreamProvider::captured,
                this,
                [this](AudioStream captured) {
                    if (m_impl->freeze->value())
                    {
                        return;
                    }

                    auto stereo_captured = captured.channels_cast<2>();

                    m_impl->streamProcessor.first.processStereo(
                            stereo_captured);
                    m_impl->streamProcessor.second.processStereo(
                            stereo_captured);

                    if (m_impl->modeEnum() != Mode::Free)
                    {
                        auto scopeData = m_impl->scopeDataGenerator.process(
                                m_impl->modeEnum() == Mode::TriggerB,
                                {m_impl->streamProcessor.first.scopeDataCache
                                         .cached(),
                                 m_impl->streamProcessor.second.scopeDataCache
                                         .cached()},
                                m_viewSize,
                                m_impl->triggerSlopeEnum(),
                                m_impl->triggerLevel->value(),
                                captured.num_frames(),
                                m_impl->holdTimeInFrames());

                        if (scopeData.size() == 2)
                        {
                            m_impl->streamProcessor.first.scopeData.set(
                                    scopeData[0]);
                            m_impl->streamProcessor.second.scopeData.set(
                                    scopeData[1]);
                        }
                    }
                });

        QObject::connect(
                m_impl->streamProcessor.first.active.get(),
                &BoolParameter::valueChanged,
                this,
                clear_fn);

        QObject::connect(
                m_impl->streamProcessor.first.channel.get(),
                &EnumParameter::valueChanged,
                this,
                clear_fn);

        QObject::connect(
                m_impl->streamProcessor.second.active.get(),
                &BoolParameter::valueChanged,
                this,
                clear_fn);

        QObject::connect(
                m_impl->streamProcessor.second.channel.get(),
                &EnumParameter::valueChanged,
                this,
                clear_fn);
    }

    QObject::connect(
            m_impl->mode.get(),
            &EnumParameter::valueChanged,
            this,
            [this]() {
                m_impl->updateMode();
                clear();
            });

    QObject::connect(
            m_impl->waveformResolution.get(),
            &IntParameter::valueChanged,
            this,
            [this]() { m_impl->updateWaveformGenerator(); });

    QObject::connect(
            m_impl->scopeResolution.get(),
            &IntParameter::valueChanged,
            this,
            [this]() {
                m_impl->updateScopeSamplesCache(m_viewSize);
                clear();
            });

    QObject::connect(this, &FxScope::viewSizeChanged, this, [this]() {
        m_impl->updateWaveform(m_viewSize);
        m_impl->updateScopeSamplesCache(m_viewSize);
        clear();
    });
}

auto
FxScope::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
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
    return m_impl->waveformResolution.get();
}

auto
FxScope::scopeWindowSize() const noexcept -> IntParameter*
{
    return m_impl->scopeResolution.get();
}

auto
FxScope::activeA() const noexcept -> BoolParameter*
{
    return m_impl->streamProcessor.first.active.get();
}

auto
FxScope::activeB() const noexcept -> BoolParameter*
{
    return m_impl->streamProcessor.second.active.get();
}

auto
FxScope::channelA() const noexcept -> EnumParameter*
{
    return m_impl->streamProcessor.first.channel.get();
}

auto
FxScope::channelB() const noexcept -> EnumParameter*
{
    return m_impl->streamProcessor.second.channel.get();
}

auto
FxScope::gainA() const noexcept -> FloatParameter*
{
    return m_impl->streamProcessor.first.gain.get();
}

auto
FxScope::gainB() const noexcept -> FloatParameter*
{
    return m_impl->streamProcessor.second.gain.get();
}

auto
FxScope::freeze() const noexcept -> BoolParameter*
{
    return m_impl->freeze.get();
}

auto
FxScope::waveformA() const noexcept -> WaveformSlot*
{
    return &m_impl->streamProcessor.first.waveform;
}

auto
FxScope::waveformB() const noexcept -> WaveformSlot*
{
    return &m_impl->streamProcessor.second.waveform;
}

auto
FxScope::scopeDataA() const noexcept -> ScopeData*
{
    return &m_impl->streamProcessor.first.scopeData;
}

auto
FxScope::scopeDataB() const noexcept -> ScopeData*
{
    return &m_impl->streamProcessor.second.scopeData;
}

void
FxScope::clear()
{
    m_impl->streamProcessor.first.scopeData.clear();
    m_impl->streamProcessor.second.scopeData.clear();
    m_impl->scopeDataGenerator.clear();

    m_impl->streamProcessor.first.waveform.clear();
    m_impl->streamProcessor.second.waveform.clear();
    m_impl->streamProcessor.first.waveformGenerator.reset();
    m_impl->streamProcessor.second.waveformGenerator.reset();
}

void
FxScope::onSubscribe()
{
    m_impl->updateSampleRate(
            observe_once(runtime::selectors::select_sample_rate)->current);
}

} // namespace piejam::fx_modules::scope::gui
