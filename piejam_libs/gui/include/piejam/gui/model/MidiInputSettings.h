// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>
#include <vector>

namespace piejam::gui::model
{

class MidiInputSettings : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* devices READ devices CONSTANT)

public:
    auto devices() -> MidiDeviceList* { return &m_devices; }

private:
    MidiDeviceList m_devices;
};

} // namespace piejam::gui::model
