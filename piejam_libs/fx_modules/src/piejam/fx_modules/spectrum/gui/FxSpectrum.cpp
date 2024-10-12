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
#include <piejam/renew.h>
#include <piejam/runtime/selectors.h>
#include <piejam/switch_cast.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

#include <ranges>

namespace piejam::fx_modules::spectrum::gui
{

using namespace piejam::gui::model;

struct FxSpectrum::Impl
{
    static constexpr audio::sample_rate default_sample_rate{48000u};

    Impl(BusType busType)
        : busType{busType}
    {
    }

    BusType busType;
    audio::sample_rate sample_rate{default_sample_rate};

    template <class T>
    using unipair = std::pair<T, T>;

    unipair<SpectrumDataGenerator> spectrumGenerator{
            std::piecewise_construct,
            std::forward_as_tuple(sample_rate),
            std::forward_as_tuple(sample_rate)};
    unipair<std::unique_ptr<BoolParameter>> active;
    unipair<std::unique_ptr<EnumParameter>> channel;
    unipair<std::unique_ptr<FloatParameter>> gain;
    unipair<SpectrumData> spectrumData;

    std::unique_ptr<BoolParameter> freeze;
    std::unique_ptr<FxStream> stream;

    void updateSampleRate(audio::sample_rate sr)
    {
        if (sr.valid() && sample_rate != sr)
        {
            sample_rate = sr;
            renew(spectrumGenerator.first, sr);
            renew(spectrumGenerator.second, sr);
        }
    }

    template <std::size_t I, class Samples>
    void process(Samples&& samples)
    {
        float const g = static_cast<float>(get<I>(gain)->value());
        switch (switch_cast(g))
        {
            case switch_cast(1.f):
                get<I>(spectrumData)
                        .set(get<I>(spectrumGenerator)
                                     .process(std::forward<Samples>(samples)));
                break;

            default:
                get<I>(spectrumData)
                        .set(get<I>(spectrumGenerator)
                                     .process(
                                             std::forward<Samples>(samples) |
                                             std::views::transform(
                                                     multiplies<>(g))));
                break;
        }
    }

    template <std::size_t I>
    void processStereo(StereoAudioStream stream)
    {
        if (!get<I>(active)->value())
        {
            return;
        }

        switch (static_cast<StereoChannel>(get<I>(channel)->value()))
        {
            case StereoChannel::Left:
                process<I>(stream.channels()[0]);
                break;

            case StereoChannel::Right:
                process<I>(stream.channels()[1]);
                break;

            case StereoChannel::Middle:
                process<I>(
                        stream.frames() |
                        std::views::transform(
                                StereoFrameValue<StereoChannel::Middle>{}));
                break;

            case StereoChannel::Side:
                process<I>(
                        stream.frames() |
                        std::views::transform(
                                StereoFrameValue<StereoChannel::Side>{}));
                break;
        }
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
            m_impl->active.first,
            parameters.at(to_underlying(parameter_key::stream_a_active)));

    makeParameter(
            m_impl->active.second,
            parameters.at(to_underlying(parameter_key::stream_b_active)));

    makeParameter(
            m_impl->channel.first,
            parameters.at(to_underlying(parameter_key::channel_a)));

    makeParameter(
            m_impl->channel.second,
            parameters.at(to_underlying(parameter_key::channel_b)));

    makeParameter(
            m_impl->gain.first,
            parameters.at(to_underlying(parameter_key::gain_a)));

    makeParameter(
            m_impl->gain.second,
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
                    m_impl->process<0>(captured.samples());
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
                    m_impl->processStereo<0>(stereo_captured);
                    m_impl->processStereo<1>(stereo_captured);
                });

        auto const clear_fn = [this]() { clear(); };

        QObject::connect(
                m_impl->active.first.get(),
                &BoolParameter::valueChanged,
                this,
                clear_fn);

        QObject::connect(
                m_impl->channel.first.get(),
                &EnumParameter::valueChanged,
                this,
                clear_fn);

        QObject::connect(
                m_impl->active.second.get(),
                &BoolParameter::valueChanged,
                this,
                clear_fn);

        QObject::connect(
                m_impl->channel.second.get(),
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
    return m_impl->active.first.get();
}

auto
FxSpectrum::activeB() const noexcept -> BoolParameter*
{
    return m_impl->active.second.get();
}

auto
FxSpectrum::channelA() const noexcept -> EnumParameter*
{
    return m_impl->channel.first.get();
}

auto
FxSpectrum::channelB() const noexcept -> EnumParameter*
{
    return m_impl->channel.second.get();
}

auto
FxSpectrum::gainA() const noexcept -> FloatParameter*
{
    return m_impl->gain.first.get();
}

auto
FxSpectrum::gainB() const noexcept -> FloatParameter*
{
    return m_impl->gain.second.get();
}

auto
FxSpectrum::freeze() const noexcept -> BoolParameter*
{
    return m_impl->freeze.get();
}

auto
FxSpectrum::dataA() const noexcept -> SpectrumData*
{
    return &m_impl->spectrumData.first;
}

auto
FxSpectrum::dataB() const noexcept -> SpectrumData*
{
    return &m_impl->spectrumData.second;
}

void
FxSpectrum::clear()
{
    m_impl->spectrumData.first.clear();
    m_impl->spectrumData.second.clear();
}

void
FxSpectrum::onSubscribe()
{
    m_impl->updateSampleRate(
            observe_once(runtime::selectors::select_sample_rate)->current);
}

} // namespace piejam::fx_modules::spectrum::gui
