// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MainModel.h"

#include "TaskList.h"

#include <QQuickStyle>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace
{

auto
createDataPartition() -> bool
{
    // resize last partition
    if (std::system("parted -s /dev/mmcblk0 resizepart 3 100%") != 0)
    {
        return false;
    }

    // create piejam filesystem
    if (std::system("mkfs.ext2 -q -F -L piejam /dev/mmcblk0p3") != 0)
    {
        return false;
    }

    std::error_code ec;
    if (!std::filesystem::create_directory("/piejam", ec))
    {
        return false;
    }

    if (auto fstab = std::ofstream{"/etc/fstab", std::ios_base::app}; fstab)
    {
        fstab << "/dev/mmcblk0p3	/piejam		ext2	rw,defaults	0	2"
              << std::endl;
    }
    else
    {
        return false;
    }

    if (std::system("mount /dev/mmcblk0p3 /piejam") != 0)
    {
        return false;
    }

    return true;
}

} // namespace

auto
main(int argc, char* argv[]) -> int
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    MainModel mainModel;
    mainModel.tasks()->add("Create data partition...", &createDataPartition);

    QObject::connect(
            mainModel.tasks(),
            &TaskList::finished,
            &app,
            &QGuiApplication::exit,
            Qt::QueuedConnection);

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");
    engine.rootContext()->setContextProperty("mainModel", &mainModel);

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
    {
        return -1;
    }

    QObject::connect(
            &engine,
            &QQmlApplicationEngine::quit,
            &QGuiApplication::quit);

    return app.exec();
}
