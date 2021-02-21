// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/audio/types.h>
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/runtime/device_io_fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>

#include <memory>

namespace piejam::app::gui::model
{

class MidiAssignable;

class MixerChannel final : public Subscribable<piejam::gui::model::MixerChannel>
{
public:
    MixerChannel(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::bus_id,
            runtime::float_parameter_id volume,
            runtime::float_parameter_id pan_balance,
            runtime::bool_parameter_id mute,
            runtime::stereo_level_parameter_id level);
    ~MixerChannel();

    void changeName(QString const&) override;
    void changeVolume(double) override;
    void changePanBalance(double) override;
    void changeMute(bool) override;
    void focusFxChain() override;
    void deleteChannel() override;

    auto volumeMidi() const -> piejam::gui::model::MidiAssignable* override;
    auto panMidi() const -> piejam::gui::model::MidiAssignable* override;
    auto muteMidi() const -> piejam::gui::model::MidiAssignable* override;

    void changeInputToMix() override;
    void changeInputToDevice(unsigned index) override;
    void changeInputToChannel(unsigned index) override;

    void changeOutputToNone() override;
    void changeOutputToDevice(unsigned index) override;
    void changeOutputToChannel(unsigned index) override;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
