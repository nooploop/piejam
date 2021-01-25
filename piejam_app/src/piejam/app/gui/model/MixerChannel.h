// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/MixerChannel.h>
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

    void changeVolume(double) override;
    void changePanBalance(double) override;
    void changeMute(bool) override;
    void focusFxChain() override;

    auto volumeMidi() const -> piejam::gui::model::MidiAssignable* override;

private:
    void onSubscribe() override;

    runtime::mixer::bus_id m_bus_id;
    runtime::float_parameter_id m_volume;
    runtime::float_parameter_id m_pan_balance;
    runtime::bool_parameter_id m_mute;
    runtime::stereo_level_parameter_id m_level;

    std::unique_ptr<MidiAssignable> m_volumeMidi;
};

} // namespace piejam::app::gui::model
