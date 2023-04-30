// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxSpectrum.h>

#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/SpectrumDataGenerator.h>
#include <piejam/runtime/modules/spectrum/spectrum_module.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::gui::model
{

struct FxSpectrum::Impl
{
    runtime::fx::module_id fx_mod_id;

    SpectrumDataGenerator dataGeneratorA;
    SpectrumDataGenerator dataGeneratorB;

    std::unique_ptr<AudioStreamProvider> streamA;
    std::unique_ptr<AudioStreamProvider> streamB;
};

FxSpectrum::FxSpectrum(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_mod_id))
    , m_busType{toBusType(observe_once(
              runtime::selectors::make_fx_module_bus_type_selector(fx_mod_id)))}
{
    auto const streams =
            observe_once(runtime::selectors::make_fx_module_streams_selector(
                    m_impl->fx_mod_id));

    constexpr auto streamKey =
            to_underlying(runtime::modules::spectrum::stream_key::input);
    auto const streamId = streams.get().at(streamKey);
    FxStreamKeyId fxStreamKeyId{.key = streamKey, .id = streamId};

    m_impl->streamA = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connectSubscribableChild(*m_impl->streamA);

    m_impl->streamA->setListener(&m_impl->dataGeneratorA);
    m_impl->dataGeneratorA.setBusType(m_busType);
    m_impl->dataGeneratorA.setActive(activeA());
    m_impl->dataGeneratorA.setChannel(channelA());

    QObject::connect(
            &m_impl->dataGeneratorA,
            &SpectrumDataGenerator::generated,
            this,
            [this](SpectrumDataPoints const& dataPoints) {
                dataA()->set(dataPoints);
            });

    if (m_busType == BusType::Stereo)
    {
        m_impl->streamB = std::make_unique<FxStream>(
                dispatch(),
                this->state_change_subscriber(),
                fxStreamKeyId);

        connectSubscribableChild(*m_impl->streamB);

        m_impl->streamB->setListener(&m_impl->dataGeneratorB);
        m_impl->dataGeneratorB.setBusType(m_busType);
        m_impl->dataGeneratorB.setActive(activeB());
        m_impl->dataGeneratorB.setChannel(channelB());

        QObject::connect(
                &m_impl->dataGeneratorB,
                &SpectrumDataGenerator::generated,
                this,
                [this](SpectrumDataPoints const& dataPoints) {
                    dataB()->set(dataPoints);
                });
    }
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

void
FxSpectrum::changeActiveA(bool const active)
{
    if (m_activeA != active)
    {
        m_activeA = active;
        m_impl->dataGeneratorA.setActive(activeA());
        emit activeAChanged();
    }
}

void
FxSpectrum::changeChannelA(piejam::gui::model::StereoChannel const x)
{
    if (m_channelA != x)
    {
        m_channelA = x;
        m_impl->dataGeneratorA.setChannel(channelA());
        emit channelAChanged();
    }
}

void
FxSpectrum::changeActiveB(bool const active)
{
    if (m_activeB != active)
    {
        m_activeB = active;
        m_impl->dataGeneratorB.setActive(activeB());
        emit activeBChanged();
    }
}

void
FxSpectrum::changeChannelB(piejam::gui::model::StereoChannel const x)
{
    if (m_channelB != x)
    {
        m_channelB = x;
        m_impl->dataGeneratorB.setChannel(channelB());
        emit channelBChanged();
    }
}

} // namespace piejam::gui::model
