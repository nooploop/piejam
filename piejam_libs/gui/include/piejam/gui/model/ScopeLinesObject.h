// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/ScopeLines.h>

#include <QObject>

namespace piejam::gui::model
{

class ScopeLinesObject final : public QObject
{
    Q_OBJECT

public:
    auto get() noexcept -> ScopeLines& { return m_lines; }
    auto get() const noexcept -> ScopeLines const& { return m_lines; }

    void update() { emit changed(); }

signals:
    void changed();

private:
    ScopeLines m_lines;
};

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::ScopeLinesObject*)
