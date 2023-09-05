// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "TaskList.h"

#include <QObject>

#include <memory>

class MainModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* tasks READ tasks CONSTANT FINAL)

public:
    MainModel();
    ~MainModel() override;

    auto tasks() const -> TaskList*
    {
        return m_tasks.get();
    }

private:
    std::unique_ptr<TaskList> m_tasks;
};
