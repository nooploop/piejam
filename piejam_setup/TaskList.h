// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "Task.h"

#include <QAbstractListModel>

#include <memory>
#include <vector>

class TaskList : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles : int
    {
        ItemRole = Qt::UserRole
    };

    using QAbstractListModel::QAbstractListModel;

    auto rowCount(QModelIndex const& = QModelIndex()) const -> int override
    {
        return static_cast<int>(m_tasks.size());
    }

    auto data(QModelIndex const& index, int const role = Qt::DisplayRole) const
            -> QVariant override
    {
        switch (role)
        {
            case ItemRole:
                return QVariant::fromValue(m_tasks[index.row()].get());
            default:
                return {};
        }
    }

    auto roleNames() const -> QHash<int, QByteArray> override
    {
        static QHash<int, QByteArray> const s_roles = {
                {Qt::DisplayRole, "disconst play"},
                {ItemRole, "item"}};
        return s_roles;
    }

    template <class T>
    void add(QString description, T&& task)
    {
        auto* prev = m_tasks.empty() ? nullptr : m_tasks.back().get();
        auto& next = *m_tasks.emplace_back(std::make_unique<Task>(
                std::move(description),
                std::forward<T>(task)));

        if (prev)
        {
            QObject::connect(
                    prev,
                    &Task::statusChanged,
                    &next,
                    [prev, &next]() {
                        switch (prev->status())
                        {
                            case Task::Status::Done:
                                next.run();
                                break;

                            case Task::Status::Running:
                                break;

                            default:
                                next.skip();
                                break;
                        }
                    });
        }

        QObject::disconnect(m_finished);
        m_finished = QObject::connect(
                &next,
                &Task::statusChanged,
                this,
                &TaskList::onLastTaskExecuted);
    }

    Q_INVOKABLE void run()
    {
        if (!m_tasks.empty())
        {
            m_tasks.front()->run();
        }
    }

signals:
    void finished(int retcode);

private:
    void onLastTaskExecuted()
    {
        switch (m_tasks.back()->status())
        {
            case Task::Status::Done:
                emit finished(0);
                break;

            case Task::Status::Failed:
            case Task::Status::Skipped:
                emit finished(1);
                break;

            default:
                break;
        }
    }

    std::vector<std::unique_ptr<Task>> m_tasks;
    QMetaObject::Connection m_finished;
};
