// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/MixerChannel.h>

#include <piejam/runtime/mixer_fwd.h>

#include <QStringList>

#include <memory>

namespace piejam::gui::model
{

class MixerChannelEdit final : public MixerChannel
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(bool, canMoveLeft, setCanMoveLeft)
    M_PIEJAM_GUI_PROPERTY(bool, canMoveRight, setCanMoveRight)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::AudioRouting*, in)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::AudioRouting*, out)

public:
    MixerChannelEdit(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannelEdit() override;

    Q_INVOKABLE void changeName(QString const&);
    Q_INVOKABLE void changeColor(MaterialColor);
    Q_INVOKABLE void moveLeft();
    Q_INVOKABLE void moveRight();
    Q_INVOKABLE void deleteChannel();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
