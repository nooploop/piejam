// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QtConcurrent/QtConcurrent>

#include <cassert>
#include <functional>

class Task : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString description READ description CONSTANT FINAL)

public:
    template <class T>
    Task(QString description, T&& task)
        : m_description{std::move(description)}
        , m_task{std::forward<T>(task)}
    {
        QObject::connect(
                &m_watcher,
                &QFutureWatcher<bool>::finished,
                this,
                [&]() {
                    setStatus(
                            m_watcher.result() ? Status::Done : Status::Failed);
                });
    }

    auto description() const noexcept -> QString const&
    {
        return m_description;
    }

    void run()
    {
        assert(m_status == Status::Initial);
        setStatus(Status::Running);
        m_watcher.setFuture(QtConcurrent::run(std::move(m_task)));
    }

    enum class Status : int
    {
        Initial,
        Running,
        Done,
        Failed,
        Skipped
    };

    Q_ENUM(Status);

    Q_PROPERTY(Status status READ status NOTIFY statusChanged FINAL)

    auto status() const noexcept -> Status
    {
        return m_status;
    }

    void skip()
    {
        setStatus(Status::Skipped);
    }

signals:
    void statusChanged();

private:
    void setStatus(Status x)
    {
        if (m_status != x)
        {
            m_status = x;
            emit statusChanged();
        }
    }

    QString m_description;
    std::function<bool()> m_task;
    QFutureWatcher<bool> m_watcher;
    Status m_status{Status::Initial};
};
