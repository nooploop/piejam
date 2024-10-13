// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/spectrum/gui/FxSpectrum.h>

#include <piejam/fx_modules/spectrum/spectrum_internal_id.h>
#include <piejam/fx_modules/spectrum/spectrum_module.h>

#include <piejam/functional/operators.h>
#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/EnumParameter.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/SpectrumDataGenerator.h>
#include <piejam/gui/model/StreamProcessor.h>
#include <piejam/renew.h>
#include <piejam/runtime/selectors.h>
#include <piejam/switch_cast.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>


namespace piejam::fx_modules::spectrum::gui
{

using namespace piejam::gui::model;

namespace
{

struct SpectrumProcessor : StreamProcessor<SpectrumProcessor>
{
    static constexpr audio::sample_rate default_sample_rate{48000u};

    template <class Samples>
    void process(Samples&& samples)
    {
        spectrumData.set(
                spectrumGenerator.process(std::forward<Samples>(samples)));
    }

    void updateSampleRate(audio::sample_rate sr)
    {
        if (sr.valid() && sample_rate != sr)
        {
            renew(spectrumGenerator, sr);
            sample_rate = sr;
        }
    }

    audio::sample_rate sample_rate{default_sample_rate};
    SpectrumDataGenerator spectrumGenerator{sample_rate};
    SpectrumData spectrumData;
};

} // namespace

struct FxSpectrum::Impl
{

    Impl(BusType busType)
        : busType{busType}
    {
    }

    BusType busType;

    std::pair<SpectrumProcessor, SpectrumProcessor> spectrumProcessor;

    std::unique_ptr<BoolParameter> freeze;
    std::unique_ptr<FxStream> stream;

    void updateSampleRate(audio::sample_rate sr)
    {
        spectrumProcessor.first.updateSampleRate(sr);
        spectrumProcessor.second.updateSampleRate(sr);
    }
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
            m_impl->spectrumProcessor.first.active,
            parameters.at(to_underlying(parameter_key::stream_a_active)));

    makeParameter(
            m_impl->spectrumProcessor.second.active,
            parameters.at(to_underlying(parameter_key::stream_b_active)));

    makeParameter(
            m_impl->spectrumProcessor.first.channel,
            parameters.at(to_underlying(parameter_key::channel_a)));

    makeParameter(
            m_impl->spectrumProcessor.second.channel,
            parameters.at(to_underlying(parameter_key::channel_b)));

    makeParameter(
            m_impl->spectrumProcessor.first.gain,
            parameters.at(to_underlying(parameter_key::gain_a)));

    makeParameter(
            m_impl->spectrumProcessor.second.gain,
            parameters.at(to_underlying(parameter_key::gain_b)));

    makeParameter(
            m_impl->freeze,
            parameters.at(to_underlying(parameter_key::freeze)));

    makeStream(to_underlying(stream_key::input), m_impl->stream, streams());

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
                    m_impl->spectrumProcessor.first.processSamples(
                            captured.samples());
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

                    auto captured_stereo = captured.channels_cast<2>();
                    m_impl->spectrumProcessor.first.processStereo(
                            captured_stereo);
                    m_impl->spectrumProcessor.second.processStereo(
                            captured_stereo);
                });

        auto const clear_fn = [this]() { clear(); };

        QObject::connect(
                m_impl->spectrumProcessor.first.active.get(),
                &BoolParameter::valueChanged,
                this,
                clear_fn);

        QObject::connect(
                m_impl->spectrumProcessor.first.channel.get(),
                &EnumParameter::valueChanged,
                this,
                clear_fn);

        QObject::connect(
                m_impl->spectrumProcessor.second.active.get(),
                &BoolParameter::valueChanged,
                this,
                clear_fn);

        QObject::connect(
                m_impl->spectrumProcessor.second.channel.get(),
                &EnumParameter::valueChanged,
                this,
                clear_fn);
    }
}

auto
FxSpectrum::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
}

auto
FxSpectrum::activeA() const noexcept -> BoolParameter*
{
    return m_impl->spectrumProcessor.first.active.get();
}

auto
FxSpectrum::activeB() const noexcept -> BoolParameter*
{
    return m_impl->spectrumProcessor.second.active.get();
}

auto
FxSpectrum::channelA() const noexcept -> EnumParameter*
{
    return m_impl->spectrumProcessor.first.channel.get();
}

auto
FxSpectrum::channelB() const noexcept -> EnumParameter*
{
    return m_impl->spectrumProcessor.second.channel.get();
}

auto
FxSpectrum::gainA() const noexcept -> FloatParameter*
{
    return m_impl->spectrumProcessor.first.gain.get();
}

auto
FxSpectrum::gainB() const noexcept -> FloatParameter*
{
    return m_impl->spectrumProcessor.second.gain.get();
}

auto
FxSpectrum::freeze() const noexcept -> BoolParameter*
{
    return m_impl->freeze.get();
}

auto
FxSpectrum::dataA() const noexcept -> SpectrumData*
{
    return &m_impl->spectrumProcessor.first.spectrumData;
}

auto
FxSpectrum::dataB() const noexcept -> SpectrumData*
{
    return &m_impl->spectrumProcessor.second.spectrumData;
}

void
FxSpectrum::clear()
{
    m_impl->spectrumProcessor.first.spectrumData.clear();
    m_impl->spectrumProcessor.second.spectrumData.clear();
}

void
FxSpectrum::onSubscribe()
{
    m_impl->updateSampleRate(
            observe_once(runtime::selectors::select_sample_rate)->current);
}

} // namespace piejam::fx_modules::spectrum::gui
