// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/MixerChannelEdit.h>
#include <piejam/runtime/mixer_fwd.h>

#include <memory>

namespace piejam::app::gui::model
{

class MidiAssignable;

class MixerChannelEdit final
    : public Subscribable<piejam::gui::model::MixerChannelEdit>
{
public:
    MixerChannelEdit(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::bus_id);
    ~MixerChannelEdit();

    void changeName(QString const&) override;

    void deleteChannel() override;

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
