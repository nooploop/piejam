// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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
    Q_PROPERTY(bool bipolar READ bipolar NOTIFY bipolarChanged FINAL)

public:
    Parameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::ParameterId const&);
    ~Parameter() override;

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

    auto valueString() const noexcept -> QString const&
    {
        return m_valueString;
    }

    auto bipolar() const noexcept -> bool
    {
        return m_bipolar;
    }

    auto midi() const -> MidiAssignable*;

    Q_INVOKABLE void resetToDefault();

signals:
    void nameChanged();
    void valueStringChanged();
    void bipolarChanged();

protected:
    void onSubscribe() override;

private:
    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    void setValueString(QString const& x)
    {
        if (m_valueString != x)
        {
            m_valueString = x;
            emit valueStringChanged();
        }
    }

    void setBipolar(bool x)
    {
        if (m_bipolar != x)
        {
            m_bipolar = x;
            emit bipolarChanged();
        }
    }

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    QString m_valueString;
    bool m_bipolar{};
};

auto makeParameter(
        runtime::store_dispatch,
        runtime::subscriber&,
        piejam::gui::model::ParameterId const&) -> std::unique_ptr<Parameter>;

} // namespace piejam::gui::model
