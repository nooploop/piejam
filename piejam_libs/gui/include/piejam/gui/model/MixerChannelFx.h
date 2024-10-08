// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/gui/model/StringList.h>

#include <piejam/audio/types.h>
#include <piejam/pimpl.h>
#include <piejam/runtime/mixer_fwd.h>

namespace piejam::gui::model
{

class MixerChannelFx final : public MixerChannel
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(bool, focused, setFocused)
    M_PIEJAM_GUI_READONLY_PROPERTY(QAbstractListModel*, fxChain)
    M_PIEJAM_GUI_PROPERTY(bool, canMoveUpFxModule, setCanMoveUpFxModule)
    M_PIEJAM_GUI_PROPERTY(bool, canMoveDownFxModule, setCanMoveDownFxModule)

public:
    MixerChannelFx(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);

    Q_INVOKABLE void appendFxModule();
    Q_INVOKABLE void moveUpFxModule();
    Q_INVOKABLE void moveDownFxModule();

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
