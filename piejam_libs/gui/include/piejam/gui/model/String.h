// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>

#include <piejam/entity_id.h>
#include <piejam/runtime/string_id.h>

namespace piejam::gui::model
{

class String final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(QString, value, setValue)

public:
    String(runtime::store_dispatch, runtime::subscriber&, runtime::string_id);

    Q_INVOKABLE void changeValue(QString);

private:
    void onSubscribe() override;

    runtime::string_id m_string_id;
};

} // namespace piejam::gui::model
