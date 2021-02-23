// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/Mixer.h>

namespace piejam::app::gui::model
{

class Mixer final : public Subscribable<piejam::gui::model::Mixer>
{
public:
    Mixer(runtime::store_dispatch, runtime::subscriber&);
    ~Mixer();

    auto mainChannel() const -> piejam::gui::model::MixerChannel* override;

    void addChannel(QString const& newChannelName) override;

    void setInputSolo(unsigned index) override;
    void setOutputSolo(unsigned index) override;

    void requestLevelsUpdate() override;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> const m_impl;
};

} // namespace piejam::app::gui::model
