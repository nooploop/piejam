// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class MidiInputSettings final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* devices READ devices CONSTANT)

public:
    MidiInputSettings(runtime::store_dispatch, runtime::subscriber&);
    ~MidiInputSettings();

    auto devices() -> MidiDeviceList*;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
