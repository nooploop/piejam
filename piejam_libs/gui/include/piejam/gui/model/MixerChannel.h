// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <QStringList>

namespace piejam::gui::model
{

class MixerChannel : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::MixerChannelPerform* perform READ perform
                       CONSTANT FINAL)
    Q_PROPERTY(
            piejam::gui::model::MixerChannelEdit* edit READ edit CONSTANT FINAL)

public:
    using SubscribableModel::SubscribableModel;

    virtual auto perform() const -> MixerChannelPerform* = 0;
    virtual auto edit() const -> MixerChannelEdit* = 0;
};

} // namespace piejam::gui::model
