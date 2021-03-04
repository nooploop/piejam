// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/audio/types.h>
#include <piejam/gui/model/MixerChannelPerform.h>
#include <piejam/runtime/device_io_fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>

#include <memory>

namespace piejam::app::gui::model
{

class MidiAssignable;

class MixerChannelPerform final
    : public Subscribable<piejam::gui::model::MixerChannelPerform>
{
public:
    MixerChannelPerform(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::bus_id,
            runtime::float_parameter_id volume,
            runtime::float_parameter_id panBalance,
            runtime::bool_parameter_id mute,
            runtime::bool_parameter_id solo,
            runtime::stereo_level_parameter_id level);
    ~MixerChannelPerform();

    void changeVolume(double) override;
    void changePanBalance(double) override;
    void changeMute(bool) override;
    void changeSolo(bool) override;
    void focusFxChain() override;

    auto volumeMidi() const -> piejam::gui::model::MidiAssignable* override;
    auto panMidi() const -> piejam::gui::model::MidiAssignable* override;
    auto muteMidi() const -> piejam::gui::model::MidiAssignable* override;
    auto soloMidi() const -> piejam::gui::model::MidiAssignable* override;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
