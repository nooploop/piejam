// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleType.h>

#include <QObject>

namespace piejam::gui::model
{

class FxModuleRegistryItem
{
    Q_GADGET
    Q_PROPERTY(piejam::gui::model::FxModuleType fxType MEMBER fxType)
    Q_PROPERTY(QString viewSource MEMBER viewSource)
public:
    FxModuleType fxType;
    QString viewSource;

    bool operator==(FxModuleRegistryItem const&) const noexcept = default;
};

class FxModuleRegistry : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QVector<piejam::gui::model::FxModuleRegistryItem> items READ
                       items CONSTANT FINAL)

public:
    auto items() const -> QVector<FxModuleRegistryItem> const&
    {
        return m_items;
    }

    void addItem(FxModuleType fxType, QString viewSource);

    Q_INVOKABLE int indexOf(piejam::gui::model::FxModuleType);

private:
    QVector<FxModuleRegistryItem> m_items;
};

auto fxModuleRegistrySingleton() -> FxModuleRegistry&;

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::FxModuleRegistryItem)
