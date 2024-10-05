// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>

#include <piejam/runtime/mixer_fwd.h>

namespace piejam::gui::model
{

class FxBrowserEntry : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(QString, name, setName)
    M_PIEJAM_GUI_PROPERTY(QString, section, setSection)
    M_PIEJAM_GUI_PROPERTY(QString, description, setDescription)
    M_PIEJAM_GUI_PROPERTY(QString, author, setAuthor)

public:
    using Subscribable<SubscribableModel>::Subscribable;

    Q_INVOKABLE virtual void appendModule() = 0;
};

} // namespace piejam::gui::model
