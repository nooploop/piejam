// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/fwd.h>

#include <QObject>

namespace piejam::gui::model
{

class FxChain : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* modules READ modules CONSTANT)

public:
    auto modules() -> FxModulesList* { return &m_modules; }

private:
    FxModulesList m_modules;
};

} // namespace piejam::gui::model
