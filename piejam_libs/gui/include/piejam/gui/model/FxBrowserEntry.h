// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/runtime/mixer_fwd.h>

namespace piejam::gui::model
{

class FxBrowserEntry : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString section READ section NOTIFY sectionChanged FINAL)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged
                       FINAL)
    Q_PROPERTY(QString author READ author NOTIFY authorChanged FINAL)

public:
    using Subscribable<SubscribableModel>::Subscribable;

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

    Q_INVOKABLE virtual void
    insertModule(unsigned chainIndex, unsigned pos) = 0;
    Q_INVOKABLE virtual void
    replaceModule(unsigned chainIndex, unsigned pos) = 0;

signals:
    void nameChanged();
    void sectionChanged();
    void descriptionChanged();
    void authorChanged();

protected:
    auto chainIdFromIndex(unsigned chainIndex) -> runtime::mixer::channel_id;

private:
    QString m_name;
    QString m_section;
    QString m_description;
    QString m_author;
};

} // namespace piejam::gui::model
