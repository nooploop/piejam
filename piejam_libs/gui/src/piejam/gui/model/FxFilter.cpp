// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxFilter.h>

#include <piejam/gui/model/AudioStreamProvider.h>
#include <piejam/gui/model/FxParameter.h>
#include <piejam/gui/model/FxParameterKeyId.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/FxStreamKeyId.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/StereoSpectrumDataGenerator.h>
#include <piejam/gui/model/StreamChannelsBisector.h>
#include <piejam/runtime/modules/filter/filter_module.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::gui::model
{

struct FxFilter::Impl
{
    runtime::fx::module_id fx_mod_id;

    bool spectogramActive{true};
    SpectrumData spectrumDataIn;
    SpectrumData spectrumDataOut;

    StreamChannelsBisector bisector;

    StereoSpectrumDataGenerator dataGeneratorIn;
    StereoSpectrumDataGenerator dataGeneratorOut;

    std::unique_ptr<AudioStreamProvider> inOutStream;

    std::unique_ptr<FxParameter> filterTypeParam;
    std::unique_ptr<FxParameter> cutoffParam;
    std::unique_ptr<FxParameter> resonanceParam;
};

FxFilter::FxFilter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_mod_id))
{
    auto const parameters =
            observe_once(runtime::selectors::make_fx_module_parameters_selector(
                    m_impl->fx_mod_id));

    constexpr auto filterTypeKey =
            to_underlying(runtime::modules::filter::parameter_key::type);
    auto const filterTypeId = parameters.get().at(filterTypeKey);
    m_impl->filterTypeParam = std::make_unique<FxParameter>(
            dispatch(),
            this->state_change_subscriber(),
            FxParameterKeyId{.key = filterTypeKey, .id = filterTypeId});
    connectSubscribableChild(*m_impl->filterTypeParam);

    constexpr auto cutoffKey =
            to_underlying(runtime::modules::filter::parameter_key::cutoff);
    auto const cutoffId = parameters.get().at(cutoffKey);
    m_impl->cutoffParam = std::make_unique<FxParameter>(
            dispatch(),
            this->state_change_subscriber(),
            FxParameterKeyId{.key = cutoffKey, .id = cutoffId});
    connectSubscribableChild(*m_impl->cutoffParam);

    constexpr auto resonanceKey =
            to_underlying(runtime::modules::filter::parameter_key::resonance);
    auto const resonanceId = parameters.get().at(resonanceKey);
    m_impl->resonanceParam = std::make_unique<FxParameter>(
            dispatch(),
            this->state_change_subscriber(),
            FxParameterKeyId{.key = resonanceKey, .id = resonanceId});
    connectSubscribableChild(*m_impl->resonanceParam);

    auto const streams =
            observe_once(runtime::selectors::make_fx_module_streams_selector(
                    m_impl->fx_mod_id));

    constexpr auto streamKey =
            to_underlying(runtime::modules::filter::stream_key::in_out);
    auto const streamId = streams.get().at(streamKey);

    m_impl->inOutStream = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            FxStreamKeyId{.key = streamKey, .id = streamId});

    connectSubscribableChild(*m_impl->inOutStream);

    m_impl->dataGeneratorIn.setActive(spectogramActive());
    m_impl->dataGeneratorIn.setChannel(StereoChannel::Middle);

    m_impl->dataGeneratorOut.setActive(spectogramActive());
    m_impl->dataGeneratorOut.setChannel(StereoChannel::Middle);

    QObject::connect(
            &m_impl->bisector,
            &StreamChannelsBisector::bisected,
            &m_impl->dataGeneratorOut,
            [this](AudioStreamListener::Stream const& in,
                   AudioStreamListener::Stream const& out) {
                m_impl->dataGeneratorIn.update(in);
                m_impl->dataGeneratorOut.update(out);
            });

    QObject::connect(
            &m_impl->dataGeneratorIn,
            &StereoSpectrumDataGenerator::generated,
            this,
            [this](SpectrumDataPoints const& dataPoints) {
                dataIn()->set(dataPoints);
            });

    QObject::connect(
            &m_impl->dataGeneratorOut,
            &StereoSpectrumDataGenerator::generated,
            this,
            [this](SpectrumDataPoints const& dataPoints) {
                dataOut()->set(dataPoints);
            });

    m_impl->inOutStream->setListener(&m_impl->bisector);
}

FxFilter::~FxFilter() = default;

void
FxFilter::onSubscribe()
{
    auto const& srs = observe_once(runtime::selectors::select_sample_rate);
    m_impl->dataGeneratorIn.setSampleRate(srs.second);
    m_impl->dataGeneratorOut.setSampleRate(srs.second);

    requestUpdates(std::chrono::milliseconds{16}, [this]() {
        m_impl->inOutStream->requestUpdate();
    });
}

bool
FxFilter::spectogramActive() const noexcept
{
    return m_impl->spectogramActive;
}

void
FxFilter::changeSpectogramActive(bool const active)
{
    if (m_impl->spectogramActive != active)
    {
        m_impl->spectogramActive = active;
        emit spectogramActiveChanged();
    }
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
