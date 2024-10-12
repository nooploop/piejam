// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/tuner/gui/FxTuner.h>

#include <piejam/fx_modules/tuner/tuner_internal_id.h>
#include <piejam/fx_modules/tuner/tuner_module.h>

#include <piejam/audio/pitch.h>
#include <piejam/gui/model/FxStream.h>
#include <piejam/gui/model/PitchGenerator.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::tuner::gui
{

using namespace piejam::gui::model;

struct FxTuner::Impl
{
    static constexpr audio::sample_rate default_sample_rate{48000u};

    BusType busType;
    audio::sample_rate sample_rate{default_sample_rate};

    PitchGenerator pitchGenerator{sample_rate};

    std::unique_ptr<FxStream> stream{};
};

FxTuner::FxTuner(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : FxModule{store_dispatch, state_change_subscriber, fx_mod_id}
    , m_impl{make_pimpl<Impl>(busType())}
{
    makeStream(to_underlying(stream_key::input), m_impl->stream, streams());

    QObject::connect(
            m_impl->stream.get(),
            &AudioStreamProvider::captured,
            this,
            [this](AudioStream captured) {
                float const detectedFrequency =
                        m_impl->busType == BusType::Mono
                                ? m_impl->pitchGenerator.process(
                                          captured.samples())
                                : m_impl->pitchGenerator.process(
                                          captured.channels_cast<2>().frames() |
                                          std::views::transform(
                                                  StereoFrameValue<
                                                          StereoChannel::
                                                                  Middle>{}));

                if (detectedFrequency != m_detectedFrequency)
                {
                    setDetectedFrequency(detectedFrequency);

                    if (detectedFrequency > 0.f)
                    {
                        auto pitch =
                                audio::pitch::from_frequency(detectedFrequency);

                        auto pc = [](audio::pitchclass pc) {
                            switch (pc)
                            {
                                case audio::pitchclass::A:
                                    return "A";
                                case audio::pitchclass::A_sharp:
                                    return "A#";
                                case audio::pitchclass::B:
                                    return "B";
                                case audio::pitchclass::C:
                                    return "C";
                                case audio::pitchclass::C_sharp:
                                    return "C#";
                                case audio::pitchclass::D:
                                    return "D";
                                case audio::pitchclass::D_sharp:
                                    return "D#";
                                case audio::pitchclass::E:
                                    return "E";
                                case audio::pitchclass::F:
                                    return "F";
                                case audio::pitchclass::F_sharp:
                                    return "F#";
                                case audio::pitchclass::G:
                                    return "G";
                                case audio::pitchclass::G_sharp:
                                    return "G#";
                            }

                            return "--";
                        }(pitch.pitchclass_);

                        setDetectedPitch(QString::fromStdString(
                                fmt::format("{}{}", pc, pitch.octave)));

                        setDetectedCents(
                                static_cast<int>(std::round(pitch.cents)));
                    }
                    else
                    {
                        static QString s_empty{"--"};
                        setDetectedPitch(s_empty);
                        setDetectedCents(0);
                    }
                }
            });
}

auto
FxTuner::type() const noexcept -> FxModuleType
{
    return {.id = internal_id()};
}

void
FxTuner::onSubscribe()
{
    auto const sample_rate =
            observe_once(runtime::selectors::select_sample_rate)->current;
    if (sample_rate.valid() && m_impl->sample_rate != sample_rate)
    {
        m_impl->pitchGenerator = PitchGenerator{sample_rate};
        m_impl->sample_rate = sample_rate;
    }
}

} // namespace piejam::fx_modules::tuner::gui
