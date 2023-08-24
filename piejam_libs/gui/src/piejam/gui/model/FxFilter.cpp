// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxFilter.h>

#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/FxFloatParameter.h>
#include <piejam/gui/model/FxIntParameter.h>
#include <piejam/gui/model/FxParameterKeyId.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/FxStreamKeyId.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/SpectrumDataGenerator.h>
#include <piejam/runtime/modules/filter/filter_module.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::gui::model
{

struct FxFilter::Impl
{
    Impl(BusType busType)
        : dataGenerator{std::array{busType, busType}}
    {
    }

    SpectrumDataGenerator dataGenerator;

    SpectrumData spectrumDataIn;
    SpectrumData spectrumDataOut;

    std::unique_ptr<AudioStreamProvider> inOutStream;

    std::unique_ptr<FxIntParameter> filterTypeParam;
    std::unique_ptr<FxFloatParameter> cutoffParam;
    std::unique_ptr<FxFloatParameter> resonanceParam;
};

FxFilter::FxFilter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable{store_dispatch, state_change_subscriber}
{
    auto const busType = toBusType(observe_once(
            runtime::selectors::make_fx_module_bus_type_selector(fx_mod_id)));

    m_impl = std::make_unique<Impl>(busType);

    auto const parameters = observe_once(
            runtime::selectors::make_fx_module_parameters_selector(fx_mod_id));

    constexpr auto filterTypeKey =
            to_underlying(runtime::modules::filter::parameter_key::type);
    m_impl->filterTypeParam = std::make_unique<FxIntParameter>(
            dispatch(),
            this->state_change_subscriber(),
            FxParameterKeyId{
                    .key = filterTypeKey,
                    .id = parameters.get().at(filterTypeKey)});
    connectSubscribableChild(*m_impl->filterTypeParam);

    constexpr auto cutoffKey =
            to_underlying(runtime::modules::filter::parameter_key::cutoff);
    m_impl->cutoffParam = std::make_unique<FxFloatParameter>(
            dispatch(),
            this->state_change_subscriber(),
            FxParameterKeyId{
                    .key = cutoffKey,
                    .id = parameters.get().at(cutoffKey)});
    connectSubscribableChild(*m_impl->cutoffParam);

    constexpr auto resonanceKey =
            to_underlying(runtime::modules::filter::parameter_key::resonance);
    m_impl->resonanceParam = std::make_unique<FxFloatParameter>(
            dispatch(),
            this->state_change_subscriber(),
            FxParameterKeyId{
                    .key = resonanceKey,
                    .id = parameters.get().at(resonanceKey)});
    connectSubscribableChild(*m_impl->resonanceParam);

    auto const streams = observe_once(
            runtime::selectors::make_fx_module_streams_selector(fx_mod_id));

    constexpr auto streamKey =
            to_underlying(runtime::modules::filter::stream_key::in_out);
    auto const streamId = streams.get().at(streamKey);

    m_impl->inOutStream = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            FxStreamKeyId{.key = streamKey, .id = streamId});

    connectSubscribableChild(*m_impl->inOutStream);

    auto stereoChannel = busType == BusType::Mono ? StereoChannel::Left
                                                  : StereoChannel::Middle;
    m_impl->dataGenerator.setActive(0, true);
    m_impl->dataGenerator.setChannel(0, stereoChannel);

    m_impl->dataGenerator.setActive(1, true);
    m_impl->dataGenerator.setChannel(1, stereoChannel);

    QObject::connect(
            &m_impl->dataGenerator,
            &SpectrumDataGenerator::generated,
            this,
            [this](std::span<SpectrumDataPoints const> const dataPoints) {
                m_impl->spectrumDataIn.set(dataPoints[0]);
                m_impl->spectrumDataOut.set(dataPoints[1]);
            });

    QObject::connect(
            m_impl->inOutStream.get(),
            &AudioStreamProvider::captured,
            &m_impl->dataGenerator,
            &SpectrumDataGenerator::update);
}

FxFilter::~FxFilter() = default;

void
FxFilter::onSubscribe()
{
    m_impl->dataGenerator.setSampleRate(
            observe_once(runtime::selectors::select_sample_rate).second);
}

auto
FxFilter::dataIn() noexcept -> SpectrumData*
{
    return &m_impl->spectrumDataIn;
}

auto
FxFilter::dataOut() noexcept -> SpectrumData*
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
FxFilter::filterType() -> FxParameter*
{
    return m_impl->filterTypeParam.get();
}

auto
FxFilter::cutoff() -> FxParameter*
{
    return m_impl->cutoffParam.get();
}

auto
FxFilter::resonance() -> FxParameter*
{
    return m_impl->resonanceParam.get();
}

} // namespace piejam::gui::model
