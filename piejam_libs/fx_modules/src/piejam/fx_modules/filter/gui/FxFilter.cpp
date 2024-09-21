// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/filter/gui/FxFilter.h>

#include <piejam/fx_modules/filter/filter_internal_id.h>
#include <piejam/fx_modules/filter/filter_module.h>
#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/EnumParameter.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/ParameterId.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/SpectrumDataGenerator.h>
#include <piejam/gui/model/StreamId.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::filter::gui
{

using namespace piejam::gui::model;

struct FxFilter::Impl
{
    Impl(BusType busType)
        : dataGenerator{std::array{busType, busType}}
    {
    }

    SpectrumDataGenerator dataGenerator;

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
    : Subscribable{store_dispatch, state_change_subscriber}
{
    auto const busType = observe_once(
            runtime::selectors::make_fx_module_bus_type_selector(fx_mod_id));

    m_impl = std::make_unique<Impl>(toBusType(busType));

    auto const parameters = observe_once(
            runtime::selectors::make_fx_module_parameters_selector(fx_mod_id));

    makeParameter(
            m_impl->filterTypeParam,
            parameters->at(to_underlying(parameter_key::type)));

    makeParameter(
            m_impl->cutoffParam,
            parameters->at(to_underlying(parameter_key::cutoff)));

    makeParameter(
            m_impl->resonanceParam,
            parameters->at(to_underlying(parameter_key::resonance)));

    auto const streams = observe_once(
            runtime::selectors::make_fx_module_streams_selector(fx_mod_id));

    makeStream(
            to_underlying(stream_key::in_out),
            m_impl->inOutStream,
            *streams);

    auto stereoChannel =
            bus_type_to(busType, StereoChannel::Left, StereoChannel::Right);
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
            observe_once(runtime::selectors::select_sample_rate)->current);
}

auto
FxFilter::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
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
FxFilter::filterType() -> EnumParameter*
{
    return m_impl->filterTypeParam.get();
}

auto
FxFilter::cutoff() -> FloatParameter*
{
    return m_impl->cutoffParam.get();
}

auto
FxFilter::resonance() -> FloatParameter*
{
    return m_impl->resonanceParam.get();
}

} // namespace piejam::fx_modules::filter::gui
