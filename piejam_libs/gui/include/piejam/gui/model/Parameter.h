// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/ParameterId.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class Parameter : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(Type type READ type CONSTANT FINAL)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString valueString READ valueString NOTIFY valueStringChanged
                       FINAL)
    Q_PROPERTY(
            piejam::gui::model::MidiAssignable* midi READ midi CONSTANT FINAL)

public:
    Parameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::ParameterId const&);
    ~Parameter();

    auto paramId() const -> ParameterId;

    enum class Type
    {
        Float,
        Int,
        Bool,
        Enum,
    };

    Q_ENUM(Type)

    virtual auto type() const noexcept -> Type;

    auto name() const noexcept -> QString const&
    {
        return m_name;
    }

    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    auto valueString() const noexcept -> QString const&
    {
        return m_valueString;
    }

    void setValueString(QString const& x)
    {
        if (m_valueString != x)
        {
            m_valueString = x;
            emit valueStringChanged();
        }
    }

    auto midi() const -> MidiAssignable*;

    Q_INVOKABLE void resetToDefault();

signals:
    void nameChanged();
    void valueStringChanged();

protected:
    void onSubscribe() override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    QString m_valueString;
};

auto makeParameter(
        runtime::store_dispatch,
        runtime::subscriber&,
        piejam::gui::model::ParameterId const&) -> std::unique_ptr<Parameter>;

} // namespace piejam::gui::model
