// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxSpectrum.h>

#include <piejam/app/gui/model/FxStream.h>
#include <piejam/gui/model/StereoSpectrumDataGenerator.h>
#include <piejam/runtime/fx/spectrum.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::app::gui::model
{

struct FxSpectrum::Impl
{
    runtime::fx::module_id fx_mod_id;

    piejam::gui::model::StereoSpectrumDataGenerator dataGeneratorA;
    piejam::gui::model::StereoSpectrumDataGenerator dataGeneratorB;

    std::unique_ptr<piejam::gui::model::AudioStreamProvider> stream;
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
            to_underlying(runtime::fx::spectrum_stream_key::left_right);
    auto const streamId = streams.get().at(streamKey);
    FxStreamKeyId fxStreamKeyId{.key = streamKey, .id = streamId};

    m_impl->stream = std::make_unique<FxStream>(
            dispatch(),
            this->state_change_subscriber(),
            fxStreamKeyId);

    connect(*m_impl->stream);

    m_impl->stream->setListener(&m_impl->dataGeneratorA);
    m_impl->dataGeneratorA.setActive(activeA());
    m_impl->dataGeneratorA.setChannel(channelA());

    QObject::connect(
            &m_impl->dataGeneratorA,
            &piejam::gui::model::StereoSpectrumDataGenerator::generated,
            [this](piejam::gui::model::SpectrumDataPoints const& dataPoints) {
                dataA()->set(dataPoints);
            });

    m_impl->dataGeneratorB.setActive(activeB());
    m_impl->dataGeneratorB.setChannel(channelB());

    QObject::connect(
            &m_impl->dataGeneratorB,
            &piejam::gui::model::StereoSpectrumDataGenerator::generated,
            [this](piejam::gui::model::SpectrumDataPoints const& dataPoints) {
                dataB()->set(dataPoints);
            });
}

FxSpectrum::~FxSpectrum() = default;

void
FxSpectrum::requestUpdate()
{
    m_impl->stream->requestUpdate();
}

void
FxSpectrum::onActiveAChanged()
{
    m_impl->dataGeneratorA.setActive(activeA());
}

void
FxSpectrum::onActiveBChanged()
{
    m_impl->dataGeneratorB.setActive(activeB());
}

void
FxSpectrum::onChannelAChanged()
{
    m_impl->dataGeneratorA.setChannel(channelA());
}

void
FxSpectrum::onChannelBChanged()
{
    m_impl->dataGeneratorB.setChannel(channelB());
}

} // namespace piejam::app::gui::model
