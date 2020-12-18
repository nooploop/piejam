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

class FxBrowserEntry : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString section READ section NOTIFY sectionChanged FINAL)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged
                       FINAL)
    Q_PROPERTY(QString author READ author NOTIFY authorChanged FINAL)

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

    auto section() const noexcept -> QString const& { return m_section; }
    void setSection(QString const& x)
    {
        if (m_section != x)
        {
            m_section = x;
            emit sectionChanged();
        }
    }

    auto description() const noexcept -> QString const&
    {
        return m_description;
    }

    void setDescription(QString const& x)
    {
        if (m_description != x)
        {
            m_description = x;
            emit descriptionChanged();
        }
    }

    auto author() const noexcept -> QString const& { return m_author; }

    void setAuthor(QString const& x)
    {
        if (m_author != x)
        {
            m_author = x;
            emit authorChanged();
        }
    }

    Q_INVOKABLE virtual void insertModule(unsigned pos) = 0;
    Q_INVOKABLE virtual void replaceModule(unsigned pos) = 0;

    auto toQString() const -> QString { return m_name; }

signals:
    void nameChanged();
    void sectionChanged();
    void descriptionChanged();
    void authorChanged();

private:
    QString m_name;
    QString m_section;
    QString m_description;
    QString m_author;
};

} // namespace piejam::gui::model
