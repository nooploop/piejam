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

#include <piejam/gui/model/SubscribableModel.h>

namespace piejam::gui::model
{

class FxParameter : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(double value READ value NOTIFY valueChanged FINAL)
    Q_PROPERTY(double min READ min NOTIFY minChanged FINAL)
    Q_PROPERTY(double max READ max NOTIFY maxChanged FINAL)

public:
    using SubscribableModel::SubscribableModel;

    auto name() const noexcept -> QString const& { return m_name; }
    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    auto value() const noexcept -> double { return m_value; }
    void setValue(double x)
    {
        if (m_value != x)
        {
            m_value = x;
            emit valueChanged();
        }
    }

    auto min() const noexcept -> double { return m_min; }
    void setMin(double x)
    {
        if (m_min != x)
        {
            m_min = x;
            emit minChanged();
        }
    }

    auto max() const noexcept -> double { return m_max; }
    void setMax(double x)
    {
        if (m_max != x)
        {
            m_max = x;
            emit valueChanged();
        }
    }

    auto toQString() const -> QString { return m_name; }

signals:
    void nameChanged();
    void valueChanged();
    void minChanged();
    void maxChanged();

private:
    QString m_name;
    double m_value{};
    double m_min{};
    double m_max{};
};

} // namespace piejam::gui::model
