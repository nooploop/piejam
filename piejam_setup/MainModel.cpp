// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MainModel.h"

#include "Task.h"
#include "TaskList.h"

#include <QGuiApplication>
#include <QtConcurrent/QtConcurrent>
#include <QtQml>

#include <mutex>
#include <thread>

namespace
{

static void
runRegistration()
{
    qRegisterMetaType<Task*>();

    qmlRegisterUncreatableType<Task>(
            "PieJamSetup",
            1,
            0,
            "Task",
            "Not creatable");
}

} // namespace

MainModel::MainModel()
    : m_tasks{std::make_unique<TaskList>()}
{
    static std::once_flag s_registered;
    std::call_once(s_registered, &runRegistration);
}

MainModel::~MainModel() = default;
