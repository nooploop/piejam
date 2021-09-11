// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxSpectrum.h>

#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/StereoSpectrumDataGenerator.h>
#include <piejam/runtime/modules/spectrum/spectrum_module.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::gui::model
{

struct FxSpectrum::Impl
{
    runtime::fx::module_id fx_mod_id;

    StereoSpectrumDataGenerator dataGeneratorA;
    StereoSpectrumDataGenerator dataGeneratorB;

    std::unique_ptr<AudioStreamProvider> streamA;
    std::unique_ptr<AudioStreamProvider> streamB;
};

FxSpectrum::FxSpectrum(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_mod_id))
{
    auto const streams =
            observe_once(runtime::selectors::make_fx_module_streams_selector(
                    m_impl->fx_mod_id));

    constexpr auto streamKey =
            to_underlying(runtime::modules::spectrum::stream_key::left_right);
    auto const streamId = streams.get().at(streamKey);
    FxStreamKeyId fxStreamKeyId{.key = streamKey, .id = streamId};

    m_impl->streamA = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connect(*m_impl->streamA);

    m_impl->streamA->setListener(&m_impl->dataGeneratorA);
    m_impl->dataGeneratorA.setActive(activeA());
    m_impl->dataGeneratorA.setChannel(channelA());

    QObject::connect(
            &m_impl->dataGeneratorA,
            &StereoSpectrumDataGenerator::generated,
            this,
            [this](SpectrumDataPoints const& dataPoints) {
                dataA()->set(dataPoints);
            });

    m_impl->streamB = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connect(*m_impl->streamB);

    m_impl->streamB->setListener(&m_impl->dataGeneratorB);
    m_impl->dataGeneratorB.setActive(activeB());
    m_impl->dataGeneratorB.setChannel(channelB());

    QObject::connect(
            &m_impl->dataGeneratorB,
            &StereoSpectrumDataGenerator::generated,
            this,
            [this](SpectrumDataPoints const& dataPoints) {
                dataB()->set(dataPoints);
            });

    QObject::connect(this, &FxSpectrum::activeAChanged, this, [this]() {
        m_impl->dataGeneratorA.setActive(activeA());
    });

    QObject::connect(this, &FxSpectrum::channelAChanged, this, [this]() {
        m_impl->dataGeneratorA.setChannel(channelA());
    });

    QObject::connect(this, &FxSpectrum::activeBChanged, this, [this]() {
        m_impl->dataGeneratorB.setActive(activeB());
    });

    QObject::connect(this, &FxSpectrum::channelBChanged, this, [this]() {
        m_impl->dataGeneratorB.setChannel(channelB());
    });
}

FxSpectrum::~FxSpectrum() = default;

void
FxSpectrum::onSubscribe()
{
    observe(runtime::selectors::select_sample_rate, [this](auto const& srs) {
        m_impl->dataGeneratorA.setSampleRate(srs.second);
        m_impl->dataGeneratorB.setSampleRate(srs.second);
    });

    requestUpdates(std::chrono::milliseconds{16}, [this]() {
        m_impl->streamA->requestUpdate();
        // no need to update streamB, since it points to same stream as streamA
    });
}

} // namespace piejam::gui::model
