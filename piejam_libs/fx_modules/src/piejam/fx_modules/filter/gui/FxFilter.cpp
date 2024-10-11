// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/filter/gui/FxFilter.h>

#include <piejam/fx_modules/filter/filter_internal_id.h>
#include <piejam/fx_modules/filter/filter_module.h>

#include <piejam/gui/model/AudioStreamChannelSplitter.h>
#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/EnumParameter.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/SpectrumDataGenerator.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::filter::gui
{

using namespace piejam::gui::model;

struct FxFilter::Impl
{
    Impl(BusType busType)
        : busType{busType}
    {
    }

    BusType busType;

    AudioStreamChannelSplitter channelSplitter{std::array{busType, busType}};

    SpectrumDataGenerator dataInGenerator{std::array{busType}};
    SpectrumDataGenerator dataOutGenerator{std::array{busType}};

    SpectrumData spectrumDataIn;
    SpectrumData spectrumDataOut;

    std::unique_ptr<EnumParameter> filterTypeParam;
    std::unique_ptr<FloatParameter> cutoffParam;
    std::unique_ptr<FloatParameter> resonanceParam;
    std::unique_ptr<FxStream> inOutStream;
};

FxFilter::FxFilter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : FxModule{store_dispatch, state_change_subscriber, fx_mod_id}
    , m_impl{make_pimpl<Impl>(toBusType(
              observe_once(runtime::selectors::make_fx_module_bus_type_selector(
                      fx_mod_id))))}
{
    auto const busType = toBusType(m_impl->busType);

    auto const& parameters = this->parameters();

    makeParameter(
            m_impl->filterTypeParam,
            parameters.at(to_underlying(parameter_key::type)));

    makeParameter(
            m_impl->cutoffParam,
            parameters.at(to_underlying(parameter_key::cutoff)));

    makeParameter(
            m_impl->resonanceParam,
            parameters.at(to_underlying(parameter_key::resonance)));

    makeStream(
            to_underlying(stream_key::in_out),
            m_impl->inOutStream,
            streams());

    auto stereoChannel =
            bus_type_to(busType, StereoChannel::Left, StereoChannel::Middle);
    m_impl->dataInGenerator.setActive(0, true);
    m_impl->dataInGenerator.setChannel(0, stereoChannel);

    m_impl->dataOutGenerator.setActive(0, true);
    m_impl->dataOutGenerator.setChannel(0, stereoChannel);

    QObject::connect(
            m_impl->inOutStream.get(),
            &AudioStreamProvider::captured,
            &m_impl->channelSplitter,
            &AudioStreamChannelSplitter::update);

    QObject::connect(
            &m_impl->channelSplitter,
            &AudioStreamChannelSplitter::splitted,
            this,
            [this](std::size_t substreamIndex, AudioStream stream) {
                switch (substreamIndex)
                {
                    case 0:
                        m_impl->dataInGenerator.update(stream);
                        break;

                    case 1:
                        m_impl->dataOutGenerator.update(stream);
                        break;

                    default:
                        BOOST_ASSERT(false);
                }
            });

    QObject::connect(
            &m_impl->dataInGenerator,
            &SpectrumDataGenerator::generated,
            this,
            [this](std::span<SpectrumDataPoints const> const dataPoints) {
                m_impl->spectrumDataIn.set(dataPoints[0]);
            });

    QObject::connect(
            &m_impl->dataOutGenerator,
            &SpectrumDataGenerator::generated,
            this,
            [this](std::span<SpectrumDataPoints const> const dataPoints) {
                m_impl->spectrumDataOut.set(dataPoints[0]);
            });
}

void
FxFilter::onSubscribe()
{
    auto const sample_rate =
            observe_once(runtime::selectors::select_sample_rate)->current;
    m_impl->dataInGenerator.setSampleRate(sample_rate);
    m_impl->dataOutGenerator.setSampleRate(sample_rate);
}

auto
FxFilter::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
}

auto
FxFilter::dataIn() const noexcept -> SpectrumData*
{
    return &m_impl->spectrumDataIn;
}

auto
FxFilter::dataOut() const noexcept -> SpectrumData*
{
    return &m_impl->spectrumDataOut;
}

void
FxFilter::clear()
{
    m_impl->spectrumDataIn.clear();
    m_impl->spectrumDataOut.clear();
}

auto
FxFilter::filterType() const noexcept -> EnumParameter*
{
    return m_impl->filterTypeParam.get();
}

auto
FxFilter::cutoff() const noexcept -> FloatParameter*
{
    return m_impl->cutoffParam.get();
}

auto
FxFilter::resonance() const noexcept -> FloatParameter*
{
    return m_impl->resonanceParam.get();
}

} // namespace piejam::fx_modules::filter::gui
